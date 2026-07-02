from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, Sequence

from PIL import Image, ImageDraw
import shapefile


ROOT = Path(__file__).resolve().parents[5]
PIPELINE_DIR = Path(__file__).resolve().parent
DATA_DIR = PIPELINE_DIR / "data"
OUTPUT_DIR = ROOT / "src" / "apps" / "geography" / "us_states_quiz" / "generated"
ARTIFACT_DIR = ROOT / "src" / "apps" / "geography" / "us_states_quiz" / "artifacts"
HEADER_FILE = OUTPUT_DIR / "UsStatesVectorData.h"
SOURCE_FILE = OUTPUT_DIR / "UsStatesVectorData.cpp"
PREVIEW_FILE = ARTIFACT_DIR / "census_cpp_grid_preview.png"

GRID_W = 240
GRID_H = 150
NONE = 255

EXCLUDED_STUSPS = {"AK", "HI", "DC", "PR", "GU", "VI", "AS", "MP"}
QUIZ_STATE_NAMES = {"Washington", "California", "Texas", "South Dakota", "Missouri"}
PREVIEW_SCALE = 3

BACKGROUND = (241, 244, 247)
BASE_MAP = (47, 52, 55)
BORDER = (247, 247, 247)
QUIZ_COLORS = {
    "Washington": (94, 128, 201),
    "California": (115, 184, 109),
    "Texas": (232, 154, 71),
    "South Dakota": (181, 163, 78),
    "Missouri": (119, 180, 95),
}

Point = tuple[float, float]
Ring = list[Point]


@dataclass
class StateShape:
    name: str
    abbrev: str
    statefp: str
    state_id: int
    rings: list[Ring]


def find_shapefile() -> Path | None:
    expected = DATA_DIR / "cb_2024_us_state_20m" / "cb_2024_us_state_20m.shp"
    if expected.exists():
        return expected
    candidates = sorted(DATA_DIR.rglob("*.shp"))
    return candidates[0] if candidates else None


def shape_to_rings(shape: shapefile.Shape) -> list[Ring]:
    points = shape.points
    parts = list(shape.parts) + [len(points)]
    rings: list[Ring] = []
    for index in range(len(parts) - 1):
        ring = [(float(x), float(y)) for x, y in points[parts[index] : parts[index + 1]]]
        if len(ring) >= 3:
            rings.append(ring)
    return rings


def load_states(path: Path) -> list[StateShape]:
    reader = shapefile.Reader(str(path))
    fields = [field[0] for field in reader.fields[1:]]
    states: list[StateShape] = []

    for shape_record in reader.iterShapeRecords():
        record = dict(zip(fields, shape_record.record))
        abbrev = str(record.get("STUSPS", ""))
        if abbrev in EXCLUDED_STUSPS:
            continue
        rings = shape_to_rings(shape_record.shape)
        if not rings:
            continue
        states.append(
            StateShape(
                name=str(record.get("NAME", abbrev)),
                abbrev=abbrev,
                statefp=str(record.get("STATEFP", "")),
                state_id=0,
                rings=rings,
            )
        )

    states.sort(key=lambda state: state.name)
    for state_id, state in enumerate(states):
        state.state_id = state_id
    return states


def bounds(states: Sequence[StateShape]) -> tuple[float, float, float, float]:
    xs = [x for state in states for ring in state.rings for x, _ in ring]
    ys = [y for state in states for ring in state.rings for _, y in ring]
    return min(xs), min(ys), max(xs), max(ys)


def make_transform(states: Sequence[StateShape]):
    min_x, min_y, max_x, max_y = bounds(states)
    scale = min((GRID_W - 1) / (max_x - min_x), (GRID_H - 1) / (max_y - min_y))
    drawn_w = (max_x - min_x) * scale
    drawn_h = (max_y - min_y) * scale
    offset_x = (GRID_W - 1 - drawn_w) / 2
    offset_y = (GRID_H - 1 - drawn_h) / 2

    def project(point: Point) -> Point:
        x, y = point
        return offset_x + (x - min_x) * scale, offset_y + drawn_h - (y - min_y) * scale

    return project


def projected_states(states: Sequence[StateShape]) -> list[StateShape]:
    project = make_transform(states)
    projected: list[StateShape] = []
    for state in states:
        projected.append(
            StateShape(
                name=state.name,
                abbrev=state.abbrev,
                statefp=state.statefp,
                state_id=state.state_id,
                rings=[[project(point) for point in ring] for ring in state.rings],
            )
        )
    return projected


def point_in_ring(point: Point, ring: Ring) -> bool:
    x, y = point
    inside = False
    j = len(ring) - 1
    for i, (xi, yi) in enumerate(ring):
        xj, yj = ring[j]
        if (yi > y) != (yj > y):
            cross_x = (xj - xi) * (y - yi) / ((yj - yi) or 1e-12) + xi
            if x < cross_x:
                inside = not inside
        j = i
    return inside


def point_in_state(point: Point, state: StateShape) -> bool:
    return any(point_in_ring(point, ring) for ring in state.rings)


def rasterize_state_id_grid(states: Sequence[StateShape]) -> list[int]:
    grid = [NONE] * (GRID_W * GRID_H)
    for y in range(GRID_H):
        for x in range(GRID_W):
            point = (x + 0.5, y + 0.5)
            for state in states:
                if point_in_state(point, state):
                    grid[y * GRID_W + x] = state.state_id
                    break
    return grid


def border_cells(grid: Sequence[int]) -> list[tuple[int, int]]:
    borders: set[tuple[int, int]] = set()
    for y in range(GRID_H):
        for x in range(GRID_W):
            state_id = grid[y * GRID_W + x]
            if state_id == NONE:
                continue
            for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                nx, ny = x + dx, y + dy
                if nx < 0 or nx >= GRID_W or ny < 0 or ny >= GRID_H:
                    borders.add((x, y))
                    break
                neighbor_id = grid[ny * GRID_W + nx]
                if neighbor_id != state_id:
                    borders.add((x, y))
                    break
    return sorted(borders, key=lambda cell: (cell[1], cell[0]))


def cpp_string(value: str) -> str:
    return value.replace("\\", "\\\\").replace('"', '\\"')


def format_grid(grid: Sequence[int]) -> str:
    lines = []
    for y in range(GRID_H):
        row = grid[y * GRID_W : (y + 1) * GRID_W]
        for start in range(0, GRID_W, 24):
            lines.append("    " + ", ".join(f"{value:3d}" for value in row[start : start + 24]) + ",")
    return "\n".join(lines)


def format_cells(cells: Sequence[tuple[int, int]]) -> str:
    lines = []
    for index in range(0, len(cells), 8):
        chunk = cells[index : index + 8]
        lines.append("    " + ", ".join(f"{{{x}, {y}}}" for x, y in chunk) + ",")
    return "\n".join(lines)


def write_header() -> None:
    HEADER_FILE.write_text(
        """#pragma once

#include <stdint.h>

namespace bitslate::geo {

static constexpr uint16_t US_GRID_W = 240;
static constexpr uint16_t US_GRID_H = 150;
static constexpr uint8_t US_STATE_COUNT = 48;
static constexpr uint8_t US_STATE_NONE = 255;

struct UsStateInfo {
  const char* name;
  const char* abbrev;
  uint8_t id;
};

struct GeoCell {
  uint16_t x;
  uint16_t y;
};

extern const UsStateInfo US_STATES[US_STATE_COUNT];
extern const uint8_t US_STATE_ID_GRID[US_GRID_W * US_GRID_H];
extern const GeoCell US_BORDER_CELLS[];
extern const uint16_t US_BORDER_CELL_COUNT;

int findStateByName(const char* name);
int stateAtCell(uint16_t x, uint16_t y);

}  // namespace bitslate::geo
""",
        encoding="utf-8",
    )


def write_source(states: Sequence[StateShape], grid: Sequence[int], borders: Sequence[tuple[int, int]]) -> None:
    lines = [
        '#include "UsStatesVectorData.h"',
        "",
        "#include <cstring>",
        "",
        "namespace bitslate::geo {",
        "",
        "const UsStateInfo US_STATES[US_STATE_COUNT] = {",
    ]
    for state in states:
        lines.append(f'    {{"{cpp_string(state.name)}", "{cpp_string(state.abbrev)}", {state.state_id}}},')
    lines.extend(
        [
            "};",
            "",
            "const uint8_t US_STATE_ID_GRID[US_GRID_W * US_GRID_H] = {",
            format_grid(grid),
            "};",
            "",
            "const GeoCell US_BORDER_CELLS[] = {",
            format_cells(borders),
            "};",
            f"const uint16_t US_BORDER_CELL_COUNT = {len(borders)};",
            "",
            "int findStateByName(const char* name) {",
            "  for (uint8_t i = 0; i < US_STATE_COUNT; ++i) {",
            "    if (std::strcmp(US_STATES[i].name, name) == 0) {",
            "      return US_STATES[i].id;",
            "    }",
            "  }",
            "  return -1;",
            "}",
            "",
            "int stateAtCell(uint16_t x, uint16_t y) {",
            "  if (x >= US_GRID_W || y >= US_GRID_H) {",
            "    return -1;",
            "  }",
            "  const uint8_t stateId = US_STATE_ID_GRID[y * US_GRID_W + x];",
            "  return stateId == US_STATE_NONE ? -1 : stateId;",
            "}",
            "",
            "}  // namespace bitslate::geo",
            "",
        ]
    )
    SOURCE_FILE.write_text("\n".join(lines), encoding="utf-8")


def write_preview(states: Sequence[StateShape], grid: Sequence[int], borders: Sequence[tuple[int, int]]) -> None:
    image = Image.new("RGB", (GRID_W * PREVIEW_SCALE, GRID_H * PREVIEW_SCALE), BACKGROUND)
    draw = ImageDraw.Draw(image)
    quiz_ids = {state.state_id: QUIZ_COLORS[state.name] for state in states if state.name in QUIZ_STATE_NAMES}

    for y in range(GRID_H):
        for x in range(GRID_W):
            state_id = grid[y * GRID_W + x]
            if state_id == NONE:
                continue
            color = quiz_ids.get(state_id, BASE_MAP)
            draw.rectangle(
                (
                    x * PREVIEW_SCALE,
                    y * PREVIEW_SCALE,
                    (x + 1) * PREVIEW_SCALE - 1,
                    (y + 1) * PREVIEW_SCALE - 1,
                ),
                fill=color,
            )

    for x, y in borders:
        draw.rectangle(
            (
                x * PREVIEW_SCALE,
                y * PREVIEW_SCALE,
                (x + 1) * PREVIEW_SCALE - 1,
                (y + 1) * PREVIEW_SCALE - 1,
            ),
            fill=BORDER,
        )

    ARTIFACT_DIR.mkdir(parents=True, exist_ok=True)
    image.save(PREVIEW_FILE)


def validate(states: Sequence[StateShape], grid: Sequence[int]) -> None:
    if len(states) != 48:
        raise ValueError(f"Expected 48 contiguous states, exported {len(states)}")
    used_ids = {value for value in grid if value != NONE}
    missing = [state.name for state in states if state.state_id not in used_ids]
    if missing:
        raise ValueError(f"States missing from rasterized grid: {', '.join(missing)}")


def main() -> None:
    shapefile_path = find_shapefile()
    if shapefile_path is None:
        print("Missing Census shapefile.")
        print("Run: python src/apps/geography/us_states_quiz/vector_pipeline/import_census_states.py")
        raise SystemExit(1)

    states = load_states(shapefile_path)
    projected = projected_states(states)
    grid = rasterize_state_id_grid(projected)
    borders = border_cells(grid)
    validate(states, grid)

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    write_header()
    write_source(states, grid, borders)
    write_preview(states, grid, borders)

    print(f"Shapefile: {shapefile_path}")
    print(f"Grid: {GRID_W}x{GRID_H}")
    print(f"States exported: {len(states)}")
    print(f"Border cells: {len(borders)}")
    print(f"Generated {HEADER_FILE}")
    print(f"Generated {SOURCE_FILE}")
    print(f"Saved {PREVIEW_FILE}")


if __name__ == "__main__":
    main()

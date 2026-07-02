from __future__ import annotations

from pathlib import Path
import sys
from typing import Iterable, Sequence, Tuple

sys.path.insert(0, str(Path(__file__).resolve().parent))

import generated_state_masks as state_masks
import generated_us_map as us_map


ROOT = Path(__file__).resolve().parents[5]
OUTPUT_DIR = ROOT / "src" / "apps" / "geography" / "us_states_quiz" / "lvgl"
HEADER_FILE = OUTPUT_DIR / "GeneratedGeographyData.h"
SOURCE_FILE = OUTPUT_DIR / "GeneratedGeographyData.cpp"

EXPORT_STATES: Tuple[str, ...] = (
    "California",
    "Texas",
    "Washington",
    "South Dakota",
    "Missouri",
)

GridPoint = Tuple[int, int]


def symbol_name(name: str) -> str:
    return "".join(ch for ch in name if ch.isalnum())


def format_cells(cells: Sequence[GridPoint]) -> str:
    lines = []
    for idx in range(0, len(cells), 8):
        chunk = cells[idx : idx + 8]
        lines.append("    " + ", ".join(f"{{{x}, {y}}}" for x, y in chunk) + ",")
    return "\n".join(lines)


def sorted_cells(cells: Iterable[GridPoint]) -> list[GridPoint]:
    return sorted(cells, key=lambda point: (point[1], point[0]))


def write_header() -> None:
    HEADER_FILE.write_text(
        """#pragma once

#include <stdint.h>

struct GeoCell {
  uint8_t x;
  uint8_t y;
};

struct GeoStateMask {
  const char* name;
  const GeoCell* cells;
  uint16_t count;
};

extern const uint16_t GEO_GRID_WIDTH;
extern const uint16_t GEO_GRID_HEIGHT;
extern const GeoCell GEO_MAP_CELLS[];
extern const uint16_t GEO_MAP_CELL_COUNT;
extern const GeoStateMask GEO_STATE_MASKS[];
extern const uint8_t GEO_STATE_COUNT;

int findGeoStateAtCell(uint8_t x, uint8_t y);
""",
        encoding="utf-8",
    )


def write_source() -> None:
    map_cells: list[GridPoint] = []
    for y, row in enumerate(us_map.CELL_MASK):
        for x, value in enumerate(row):
            if value == "1":
                map_cells.append((x, y))

    missing = [name for name in EXPORT_STATES if name not in state_masks.STATE_MASKS]
    if missing:
        raise ValueError(f"Missing generated state masks: {', '.join(missing)}")

    lines = [
        '#include "GeneratedGeographyData.h"',
        "",
        f"const uint16_t GEO_GRID_WIDTH = {us_map.GRID_WIDTH};",
        f"const uint16_t GEO_GRID_HEIGHT = {us_map.GRID_HEIGHT};",
        "",
        "const GeoCell GEO_MAP_CELLS[] = {",
        format_cells(map_cells),
        "};",
        f"const uint16_t GEO_MAP_CELL_COUNT = {len(map_cells)};",
        "",
    ]

    for state_name in EXPORT_STATES:
        cells = sorted_cells(state_masks.STATE_MASKS[state_name])
        lines.extend(
            [
                f"const GeoCell GEO_{symbol_name(state_name)}_CELLS[] = {{",
                format_cells(cells),
                "};",
                "",
            ]
        )

    lines.extend(
        [
            "const GeoStateMask GEO_STATE_MASKS[] = {",
        ]
    )
    for state_name in EXPORT_STATES:
        name = symbol_name(state_name)
        count = len(state_masks.STATE_MASKS[state_name])
        lines.append(f'    {{"{state_name}", GEO_{name}_CELLS, {count}}},')
    lines.extend(
        [
            "};",
            f"const uint8_t GEO_STATE_COUNT = {len(EXPORT_STATES)};",
            "",
            "int findGeoStateAtCell(uint8_t x, uint8_t y) {",
            "  for (uint8_t stateIndex = 0; stateIndex < GEO_STATE_COUNT; ++stateIndex) {",
            "    const GeoStateMask& mask = GEO_STATE_MASKS[stateIndex];",
            "    for (uint16_t cellIndex = 0; cellIndex < mask.count; ++cellIndex) {",
            "      if (mask.cells[cellIndex].x == x && mask.cells[cellIndex].y == y) {",
            "        return stateIndex;",
            "      }",
            "    }",
            "  }",
            "  return -1;",
            "}",
            "",
        ]
    )

    SOURCE_FILE.write_text("\n".join(lines), encoding="utf-8")

    print(f"Generated {HEADER_FILE}")
    print(f"Generated {SOURCE_FILE}")
    print(f"Grid: {us_map.GRID_WIDTH}x{us_map.GRID_HEIGHT}")
    print(f"Base map cells: {len(map_cells)}")
    for state_name in EXPORT_STATES:
        print(f"{state_name}: {len(state_masks.STATE_MASKS[state_name])}")


def main() -> None:
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    write_header()
    write_source()


if __name__ == "__main__":
    main()

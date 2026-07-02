from __future__ import annotations

from pathlib import Path
import re
from typing import Iterable, Sequence, Tuple

from PIL import Image

import generated_us_map as us_map


ROOT = Path(__file__).resolve().parents[5]
LVGL_DIR = ROOT / "src" / "apps" / "geography" / "us_states_quiz" / "lvgl"
ARTIFACT_DIR = ROOT / "src" / "apps" / "geography" / "us_states_quiz" / "artifacts"
GENERATED_CPP = LVGL_DIR / "GeneratedGeographyData.cpp"

MAP_W = 390
MAP_H = 250
GRID_W = 168
GRID_H = 108

BACKGROUND = (241, 244, 247)
BASE_MAP = (47, 52, 55)
BORDER = (247, 247, 247)

GridPoint = Tuple[int, int]


def parse_cells(array_name: str) -> list[GridPoint]:
    if not GENERATED_CPP.exists():
        return []

    text = GENERATED_CPP.read_text(encoding="utf-8")
    pattern = re.compile(
        rf"const GeoCell {re.escape(array_name)}\[\] = \{{(?P<body>.*?)\}};",
        re.DOTALL,
    )
    match = pattern.search(text)
    if not match:
        return []

    return [
        (int(x), int(y))
        for x, y in re.findall(r"\{(\d+),\s*(\d+)\}", match.group("body"))
    ]


def base_map_cells() -> list[GridPoint]:
    return [
        (x, y)
        for y, row in enumerate(us_map.CELL_MASK)
        for x, value in enumerate(row)
        if value == "1"
    ]


def draw_grid_cell(pixels: Image.Image, grid_x: int, grid_y: int, color: Tuple[int, int, int]) -> None:
    x0 = grid_x * MAP_W // GRID_W
    x1 = (grid_x + 1) * MAP_W // GRID_W
    y0 = grid_y * MAP_H // GRID_H
    y1 = (grid_y + 1) * MAP_H // GRID_H

    for y in range(y0, y1):
        for x in range(x0, x1):
            pixels.putpixel((x, y), color)


def render(cells: Sequence[GridPoint], border_cells: Iterable[GridPoint], output: Path) -> None:
    image = Image.new("RGB", (MAP_W, MAP_H), BACKGROUND)

    for x, y in cells:
        draw_grid_cell(image, x, y, BASE_MAP)
    for x, y in border_cells:
        draw_grid_cell(image, x, y, BORDER)

    output.parent.mkdir(parents=True, exist_ok=True)
    image.save(output)


def main() -> None:
    map_cells = base_map_cells()
    border_cells = parse_cells("US_STATE_BORDER_CELLS")

    current_path = ARTIFACT_DIR / "map_render_current_before_border_rollback.png"
    render(map_cells, border_cells, current_path)
    print(f"Saved {current_path}")

    if border_cells:
        noisy_path = ARTIFACT_DIR / "map_render_noisy_raster_borders_attempt.png"
        render(map_cells, border_cells, noisy_path)
        print(f"Saved {noisy_path}")


if __name__ == "__main__":
    main()

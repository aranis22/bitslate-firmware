from __future__ import annotations

from typing import Any, Tuple

import generated_us_map as generated


GridPoint = Tuple[int, int]
SourcePoint = Tuple[int, int]
SourceBounds = Tuple[int, int, int, int]

GRID_WIDTH = generated.GRID_WIDTH
GRID_HEIGHT = generated.GRID_HEIGHT
GENERATED_SOURCE_BOUNDS: SourceBounds = generated.SOURCE_BOUNDS

# The colored source image is a browser capture of the same map art on a
# smaller canvas. These bounds remove the browser/header strip while preserving
# the same map extent used by generated_us_map.py.
COLOR_SOURCE_BOUNDS: SourceBounds = (0, 48, 946, 658)


def crop_size(bounds: SourceBounds) -> Tuple[int, int]:
    min_x, min_y, max_x, max_y = bounds
    return max_x - min_x + 1, max_y - min_y + 1


def crop_box(bounds: SourceBounds) -> Tuple[int, int, int, int]:
    min_x, min_y, max_x, max_y = bounds
    return min_x, min_y, max_x + 1, max_y + 1


def grid_cell_to_source_rect(grid_x: int, grid_y: int, bounds: SourceBounds) -> Tuple[int, int, int, int]:
    min_x, min_y, _, _ = bounds
    crop_w, crop_h = crop_size(bounds)
    x0 = min_x + int(grid_x * crop_w / GRID_WIDTH)
    y0 = min_y + int(grid_y * crop_h / GRID_HEIGHT)
    x1 = min_x + int((grid_x + 1) * crop_w / GRID_WIDTH)
    y1 = min_y + int((grid_y + 1) * crop_h / GRID_HEIGHT)
    return x0, y0, max(x0 + 1, x1), max(y0 + 1, y1)


def source_pixel_to_grid_cell(source_x: int, source_y: int, bounds: SourceBounds) -> GridPoint:
    min_x, min_y, _, _ = bounds
    crop_w, crop_h = crop_size(bounds)
    grid_x = int((source_x - min_x) * GRID_WIDTH / crop_w)
    grid_y = int((source_y - min_y) * GRID_HEIGHT / crop_h)
    return clamp_grid_cell(grid_x, grid_y)


def grid_cell_to_source_pixel(grid_x: int, grid_y: int, bounds: SourceBounds) -> SourcePoint:
    x0, y0, x1, y1 = grid_cell_to_source_rect(grid_x, grid_y, bounds)
    return (x0 + x1 - 1) // 2, (y0 + y1 - 1) // 2


def grid_cell_to_screen_rect(grid_x: int, grid_y: int, draw_rect: Any) -> Any:
    import pygame

    cell_w = draw_rect.width // GRID_WIDTH
    cell_h = draw_rect.height // GRID_HEIGHT
    return pygame.Rect(
        draw_rect.x + grid_x * cell_w,
        draw_rect.y + grid_y * cell_h,
        cell_w,
        cell_h,
    )


def screen_to_grid_cell(pos: Tuple[int, int], draw_rect: Any) -> GridPoint:
    cell_w = max(1, draw_rect.width // GRID_WIDTH)
    cell_h = max(1, draw_rect.height // GRID_HEIGHT)
    grid_x = int((pos[0] - draw_rect.x) / cell_w)
    grid_y = int((pos[1] - draw_rect.y) / cell_h)
    return clamp_grid_cell(grid_x, grid_y)


def clamp_grid_cell(grid_x: int, grid_y: int) -> GridPoint:
    return max(0, min(GRID_WIDTH - 1, grid_x)), max(0, min(GRID_HEIGHT - 1, grid_y))


def cell_size_for_rect(draw_rect: Any) -> Tuple[int, int]:
    return draw_rect.width // GRID_WIDTH, draw_rect.height // GRID_HEIGHT


def fit_rect(
    window_size: Tuple[int, int],
    *,
    margin_x: int,
    margin_y: int,
    title_y: int,
    title_height: int,
) -> Any:
    import pygame

    avail_w = window_size[0] - margin_x * 2
    avail_h = window_size[1] - margin_y * 2 - title_height
    cell_size = max(1, min(avail_w // GRID_WIDTH, avail_h // GRID_HEIGHT))
    draw_w = GRID_WIDTH * cell_size
    draw_h = GRID_HEIGHT * cell_size
    draw_x = (window_size[0] - draw_w) // 2
    draw_y = title_y + title_height + (avail_h - draw_h) // 2
    return pygame.Rect(draw_x, draw_y, draw_w, draw_h)

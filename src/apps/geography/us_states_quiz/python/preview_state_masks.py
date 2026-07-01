from __future__ import annotations

from pathlib import Path
from typing import Dict, Tuple

import pygame
from PIL import Image, ImageDraw, ImageFilter

import generated_us_map as generated
import generated_state_masks as masks
import map_transform


WINDOW_SIZE = (480, 320)
MARGIN_X = 20
MARGIN_Y = 24
TITLE_Y = 6
MAP_FILL_COLOR = (50, 53, 51)
MAP_EDGE_COLOR = (82, 84, 78)
BACKGROUND_COLOR = (247, 246, 241)

MASK_COLORS: Dict[str, Tuple[int, int, int]] = {
    "California": (116, 186, 111),
    "Texas": (231, 159, 82),
    "Washington": (166, 153, 78),
    "South Dakota": (94, 127, 198),
    "Alaska": (145, 190, 226),
    "Missouri": (116, 178, 96),
}

ROOT = Path(__file__).resolve().parents[5]
SOURCE_IMAGE = ROOT / "src" / "apps" / "assets" / "images" / "geography" / "usa-map-full.png"
OVERLAY_COLOR_COUNT = 32


def fit_rect(grid_w: int, grid_h: int) -> pygame.Rect:
    return map_transform.fit_rect(
        WINDOW_SIZE,
        margin_x=MARGIN_X,
        margin_y=MARGIN_Y,
        title_y=TITLE_Y,
        title_height=20,
    )


def cell_size_for_rect(draw_rect: pygame.Rect) -> Tuple[int, int]:
    return map_transform.cell_size_for_rect(draw_rect)


def draw_generated_map(screen: pygame.Surface, draw_rect: pygame.Rect) -> None:
    for row_idx, row in enumerate(generated.CELL_MASK):
        for col_idx, value in enumerate(row):
            if value != "1":
                continue
            rect = map_transform.grid_cell_to_screen_rect(col_idx, row_idx, draw_rect)
            pygame.draw.rect(screen, MAP_FILL_COLOR, rect)

    for row_idx, row in enumerate(generated.CELL_MASK):
        for col_idx, value in enumerate(row):
            if value != "1" or not is_silhouette_edge(col_idx, row_idx):
                continue
            rect = map_transform.grid_cell_to_screen_rect(col_idx, row_idx, draw_rect)
            pygame.draw.rect(screen, MAP_EDGE_COLOR, rect)


def build_map_alpha_mask(size: Tuple[int, int]) -> Image.Image:
    grid_mask = Image.new("L", (generated.GRID_WIDTH, generated.GRID_HEIGHT), 0)
    draw = ImageDraw.Draw(grid_mask)
    for row_idx, row in enumerate(generated.CELL_MASK):
        for col_idx, value in enumerate(row):
            if value == "1":
                draw.point((col_idx, row_idx), fill=255)
    return grid_mask.resize(size, Image.Resampling.NEAREST)


def build_clean_overlay_image() -> Image.Image:
    image = Image.open(SOURCE_IMAGE).convert("RGBA")
    crop = image.crop(map_transform.crop_box(map_transform.COLOR_SOURCE_BOUNDS))
    alpha_mask = build_map_alpha_mask(crop.size)

    smoothed = crop.convert("RGB").filter(ImageFilter.MedianFilter(size=5))
    smoothed = smoothed.filter(ImageFilter.SMOOTH_MORE)
    quantized = smoothed.quantize(colors=OVERLAY_COLOR_COUNT).convert("RGBA")
    quantized.putalpha(alpha_mask)
    return quantized


def load_overlay_surface(draw_rect: pygame.Rect) -> pygame.Surface:
    overlay = build_clean_overlay_image()
    scale_size = (draw_rect.width, draw_rect.height)
    overlay = overlay.resize(scale_size, Image.Resampling.BICUBIC)
    surface = pygame.image.fromstring(overlay.tobytes(), overlay.size, "RGBA").convert_alpha()
    return surface


def screen_to_grid(pos: Tuple[int, int], draw_rect: pygame.Rect) -> Tuple[int, int]:
    return map_transform.screen_to_grid_cell(pos, draw_rect)


def is_silhouette_edge(col_idx: int, row_idx: int) -> bool:
    for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
        x = col_idx + dx
        y = row_idx + dy
        if not (0 <= x < generated.GRID_WIDTH and 0 <= y < generated.GRID_HEIGHT):
            return True
        if generated.CELL_MASK[y][x] != "1":
            return True
    return False


def draw_state_masks(screen: pygame.Surface, draw_rect: pygame.Rect) -> None:
    for state_name in masks.STATE_NAMES:
        color = MASK_COLORS.get(state_name, (80, 140, 220))
        for grid_x, grid_y in masks.get_mask_for_state(state_name):
            rect = map_transform.grid_cell_to_screen_rect(grid_x, grid_y, draw_rect)
            pygame.draw.rect(screen, color, rect)
            pygame.draw.rect(screen, generated.BOUNDARY_COLOR, rect, width=1)


def is_mask_edge(mask_points: set[Tuple[int, int]], grid_x: int, grid_y: int) -> bool:
    for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
        if (grid_x + dx, grid_y + dy) not in mask_points:
            return True
    return False


def mask_label_position(mask_points: list[Tuple[int, int]], draw_rect: pygame.Rect) -> Tuple[int, int]:
    avg_x = sum(point[0] for point in mask_points) / len(mask_points)
    avg_y = sum(point[1] for point in mask_points) / len(mask_points)
    rect = map_transform.grid_cell_to_screen_rect(round(avg_x), round(avg_y), draw_rect)
    return rect.center


def draw_state_mask_borders(screen: pygame.Surface, draw_rect: pygame.Rect, font: pygame.font.Font) -> None:
    for state_name in masks.STATE_NAMES:
        mask_points = masks.get_mask_for_state(state_name)
        if not mask_points:
            continue
        mask_set = set(mask_points)
        color = MASK_COLORS.get(state_name, (80, 140, 220))
        for grid_x, grid_y in mask_points:
            if not is_mask_edge(mask_set, grid_x, grid_y):
                continue
            rect = map_transform.grid_cell_to_screen_rect(grid_x, grid_y, draw_rect)
            pygame.draw.rect(screen, (255, 255, 255), rect, width=2)
            pygame.draw.rect(screen, color, rect, width=1)

        label = font.render(state_name, True, (255, 255, 255))
        shadow = font.render(state_name, True, (0, 0, 0))
        label_x, label_y = mask_label_position(mask_points, draw_rect)
        label_rect = label.get_rect(center=(label_x, label_y))
        shadow_rect = shadow.get_rect(center=(label_x + 1, label_y + 1))
        screen.blit(shadow, shadow_rect)
        screen.blit(label, label_rect)


def draw_debug(
    screen: pygame.Surface,
    font: pygame.font.Font,
    draw_rect: pygame.Rect,
    mouse_pos: Tuple[int, int],
) -> None:
    pygame.draw.rect(screen, (30, 120, 220), draw_rect, width=1)
    pygame.draw.rect(screen, (240, 130, 25), draw_rect, width=2)

    if draw_rect.collidepoint(mouse_pos):
        grid_x, grid_y = screen_to_grid(mouse_pos, draw_rect)
        source_x, source_y = map_transform.grid_cell_to_source_pixel(
            grid_x,
            grid_y,
            map_transform.COLOR_SOURCE_BOUNDS,
        )
        state_name = masks.get_state_at_cell(grid_x, grid_y) or "None"
        debug_text = (
            f"screen={mouse_pos} grid=({grid_x},{grid_y}) "
            f"source=({source_x},{source_y}) state={state_name}"
        )
    else:
        debug_text = f"screen={mouse_pos} outside map"

    label = font.render(debug_text, True, (20, 20, 20))
    screen.blit(label, (10, WINDOW_SIZE[1] - 36))


def main() -> int:
    pygame.init()
    pygame.display.set_caption("BitSlate State Mask Preview")
    screen = pygame.display.set_mode(WINDOW_SIZE)
    clock = pygame.time.Clock()
    font = pygame.font.SysFont("arial", 18)
    small_font = pygame.font.SysFont("arial", 13)
    label_font = pygame.font.SysFont("arial", 11, bold=True)

    draw_rect = fit_rect(generated.GRID_WIDTH, generated.GRID_HEIGHT)
    overlay = load_overlay_surface(draw_rect)
    show_overlay = False
    show_borders = False
    overlay_alpha = 80
    show_debug = False

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                running = False
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_o:
                show_overlay = not show_overlay
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_b:
                show_borders = not show_borders
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_d:
                show_debug = not show_debug

        screen.fill(BACKGROUND_COLOR)
        title = font.render("American States Quiz - Generated State Masks", True, (38, 38, 38))
        screen.blit(title, (12, TITLE_Y))

        draw_generated_map(screen, draw_rect)
        if show_overlay:
            overlay.set_alpha(overlay_alpha)
            screen.blit(overlay, draw_rect.topleft)
        draw_state_masks(screen, draw_rect)
        if show_borders:
            draw_state_mask_borders(screen, draw_rect, label_font)
        pygame.draw.rect(screen, (90, 90, 90), draw_rect, width=1)

        mouse_pos = pygame.mouse.get_pos()
        hovered_state = None
        grid_text = "Mouse outside map"
        if draw_rect.collidepoint(mouse_pos):
            grid_x, grid_y = screen_to_grid(mouse_pos, draw_rect)
            hovered_state = masks.get_state_at_cell(grid_x, grid_y)
            grid_text = f"grid=({grid_x},{grid_y}) state={hovered_state or 'None'}"

        if show_debug:
            draw_debug(screen, small_font, draw_rect, mouse_pos)

        footer = small_font.render(
            f"{grid_text}  |  O overlay  B borders  D debug  Esc quit",
            True,
            (50, 50, 50),
        )
        screen.blit(footer, (10, WINDOW_SIZE[1] - 18))

        pygame.display.flip()
        clock.tick(60)

    pygame.quit()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

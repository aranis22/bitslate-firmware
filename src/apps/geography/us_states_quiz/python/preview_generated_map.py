from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Tuple

import pygame

import generated_us_map as generated


WINDOW_SIZE = (480, 320)
MARGIN_X = 20
MARGIN_Y = 24
TITLE_Y = 6
MAP_FILL_COLOR = (50, 53, 51)
MAP_EDGE_COLOR = (82, 84, 78)
BACKGROUND_COLOR = (247, 246, 241)


def fit_rect(grid_w: int, grid_h: int) -> pygame.Rect:
    avail_w = WINDOW_SIZE[0] - MARGIN_X * 2
    avail_h = WINDOW_SIZE[1] - MARGIN_Y * 2 - 18
    cell_size = max(1, min(avail_w // grid_w, avail_h // grid_h))
    draw_w = grid_w * cell_size
    draw_h = grid_h * cell_size
    draw_x = (WINDOW_SIZE[0] - draw_w) // 2
    draw_y = TITLE_Y + 20 + (avail_h - draw_h) // 2
    return pygame.Rect(draw_x, draw_y, draw_w, draw_h)


def regenerate() -> None:
    script = Path(__file__).with_name("generate_us_map_asset.py")
    subprocess.run([sys.executable, str(script)], check=True)


def is_silhouette_edge(col_idx: int, row_idx: int) -> bool:
    for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
        x = col_idx + dx
        y = row_idx + dy
        if not (0 <= x < generated.GRID_WIDTH and 0 <= y < generated.GRID_HEIGHT):
            return True
        if generated.CELL_MASK[y][x] != "1":
            return True
    return False


def draw_generated_map(screen: pygame.Surface, draw_rect: pygame.Rect) -> None:
    cell_w = draw_rect.width // generated.GRID_WIDTH
    cell_h = draw_rect.height // generated.GRID_HEIGHT

    for row_idx, row in enumerate(generated.CELL_MASK):
        for col_idx, value in enumerate(row):
            if value != "1":
                continue
            x = draw_rect.x + col_idx * cell_w
            y = draw_rect.y + row_idx * cell_h
            rect = pygame.Rect(x, y, cell_w, cell_h)
            pygame.draw.rect(screen, MAP_FILL_COLOR, rect)

    for row_idx, row in enumerate(generated.CELL_MASK):
        for col_idx, value in enumerate(row):
            if value != "1" or not is_silhouette_edge(col_idx, row_idx):
                continue
            x = draw_rect.x + col_idx * cell_w
            y = draw_rect.y + row_idx * cell_h
            rect = pygame.Rect(x, y, cell_w, cell_h)
            pygame.draw.rect(screen, MAP_EDGE_COLOR, rect)


def main() -> int:
    pygame.init()
    pygame.display.set_caption("BitSlate USA Map Generated Preview")
    screen = pygame.display.set_mode(WINDOW_SIZE)
    clock = pygame.time.Clock()
    font = pygame.font.SysFont("arial", 18)
    small_font = pygame.font.SysFont("arial", 13)

    draw_rect = fit_rect(generated.GRID_WIDTH, generated.GRID_HEIGHT)

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                running = False
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_g:
                regenerate()
                status = "Regenerated. Restart preview to reload."
                print(status)

        screen.fill(BACKGROUND_COLOR)
        title = font.render("American States Quiz - Generated USA Map Preview", True, (38, 38, 38))
        screen.blit(title, (12, TITLE_Y))

        draw_generated_map(screen, draw_rect)
        pygame.draw.rect(screen, (90, 90, 90), draw_rect, width=1)

        footer = small_font.render(
            f"Grid {generated.GRID_WIDTH}x{generated.GRID_HEIGHT}  Cells {generated.MAP_CELL_COUNT}  Boundaries {generated.BOUNDARY_CELL_COUNT}  |  G regenerate  Esc quit",
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

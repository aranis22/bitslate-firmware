from __future__ import annotations

import sys
from pathlib import Path
from typing import Iterable, Tuple

import pygame
from PIL import Image

import generated_us_map as generated
from state_polygons import STATE_POLYGONS
from us_states_model import USStatesQuizModel


WINDOW_SIZE = (720, 480)
BG_COLOR = generated.BACKGROUND_COLOR
TEXT_COLOR = (38, 38, 38)
PROMPT_COLOR = (18, 58, 121)
STATUS_OK = (26, 127, 55)
STATUS_BAD = (166, 53, 40)
DONE_COLOR = (95, 57, 156)
OUTLINE_COLOR = (65, 65, 65)
COMPLETE_COLORS = {
    "Washington": (189, 165, 73),
    "California": (108, 140, 74),
    "Texas": (203, 145, 57),
}

ROOT = Path(__file__).resolve().parents[5]
SOURCE_IMAGE = ROOT / "src" / "apps" / "assets" / "images" / "geography" / "usa-map-unselected.png"
MAP_FRAME = pygame.Rect(30, 54, 660, 360)


def fit_rect(frame: pygame.Rect, grid_w: int, grid_h: int) -> pygame.Rect:
    cell_size = max(1, min(frame.width // grid_w, frame.height // grid_h))
    draw_w = grid_w * cell_size
    draw_h = grid_h * cell_size
    return pygame.Rect(
        frame.x + (frame.width - draw_w) // 2,
        frame.y + (frame.height - draw_h) // 2,
        draw_w,
        draw_h,
    )


def grid_to_screen(point: Tuple[int, int], draw_rect: pygame.Rect) -> Tuple[int, int]:
    cell_w = draw_rect.width // generated.GRID_WIDTH
    cell_h = draw_rect.height // generated.GRID_HEIGHT
    return (
        draw_rect.x + point[0] * cell_w + cell_w // 2,
        draw_rect.y + point[1] * cell_h + cell_h // 2,
    )


def screen_to_grid(pos: Tuple[int, int], draw_rect: pygame.Rect) -> Tuple[int, int]:
    cell_w = draw_rect.width // generated.GRID_WIDTH
    cell_h = draw_rect.height // generated.GRID_HEIGHT
    grid_x = int((pos[0] - draw_rect.x) / max(1, cell_w))
    grid_y = int((pos[1] - draw_rect.y) / max(1, cell_h))
    grid_x = max(0, min(generated.GRID_WIDTH - 1, grid_x))
    grid_y = max(0, min(generated.GRID_HEIGHT - 1, grid_y))
    return grid_x, grid_y


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
            pygame.draw.rect(screen, generated.BASE_COLOR, rect)
            if generated.BOUNDARY_MASK[row_idx][col_idx] == "1":
                pygame.draw.rect(screen, generated.BOUNDARY_COLOR, rect, width=1)


def load_overlay_surface(draw_rect: pygame.Rect) -> pygame.Surface:
    image = Image.open(SOURCE_IMAGE).convert("RGBA")
    min_x, min_y, max_x, max_y = generated.SOURCE_BOUNDS
    crop = image.crop((min_x, min_y, max_x + 1, max_y + 1))
    temp_path = Path(__file__).with_name("_quiz_overlay_temp.png")
    crop.save(temp_path)
    try:
        surface = pygame.image.load(str(temp_path)).convert_alpha()
    finally:
        temp_path.unlink(missing_ok=True)
    return pygame.transform.smoothscale(surface, draw_rect.size)


def draw_polygon_outline(screen: pygame.Surface, draw_rect: pygame.Rect, points: Iterable[Tuple[int, int]], color: Tuple[int, int, int], width: int = 2) -> None:
    screen_points = [grid_to_screen(point, draw_rect) for point in points]
    if len(screen_points) >= 2:
        pygame.draw.lines(screen, color, True, screen_points, width=width)


def draw_completed_states(screen: pygame.Surface, draw_rect: pygame.Rect, model: USStatesQuizModel) -> None:
    cell_w = draw_rect.width // generated.GRID_WIDTH
    cell_h = draw_rect.height // generated.GRID_HEIGHT

    for state_name in model.completed_states:
        color = COMPLETE_COLORS.get(state_name, (130, 170, 110))
        for grid_y in range(generated.GRID_HEIGHT):
            for grid_x in range(generated.GRID_WIDTH):
                if model.get_state_at_grid_cell(grid_x, grid_y) != state_name:
                    continue
                x = draw_rect.x + grid_x * cell_w
                y = draw_rect.y + grid_y * cell_h
                rect = pygame.Rect(x, y, cell_w, cell_h)
                pygame.draw.rect(screen, color, rect)
                if generated.BOUNDARY_MASK[grid_y][grid_x] == "1":
                    pygame.draw.rect(screen, generated.BOUNDARY_COLOR, rect, width=1)


def main() -> int:
    pygame.init()
    pygame.display.set_caption("BitSlate American States Quiz")
    screen = pygame.display.set_mode(WINDOW_SIZE)
    clock = pygame.time.Clock()

    font = pygame.font.SysFont("arial", 26)
    small_font = pygame.font.SysFont("arial", 18)
    tiny_font = pygame.font.SysFont("arial", 14)

    draw_rect = fit_rect(MAP_FRAME, generated.GRID_WIDTH, generated.GRID_HEIGHT)
    overlay_surface = load_overlay_surface(draw_rect)
    overlay_surface.set_alpha(90)

    model = USStatesQuizModel()

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                running = False
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_r:
                model.reset()
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                if draw_rect.collidepoint(event.pos) and model.target_state is not None:
                    grid_x, grid_y = screen_to_grid(event.pos, draw_rect)
                    clicked_state = model.check_click(grid_x, grid_y)
                    print(
                        f"screen=({event.pos[0]},{event.pos[1]}) "
                        f"grid=({grid_x},{grid_y}) clicked_state={clicked_state} target={model.target_state}"
                    )

        screen.fill(BG_COLOR)

        prompt_text = f"Tap {model.target_state}" if model.target_state else "Done"
        prompt_surface = font.render(prompt_text, True, PROMPT_COLOR)
        screen.blit(prompt_surface, (28, 12))

        if model.status_text == "Correct":
            status_color = STATUS_OK
        elif model.status_text == "Done":
            status_color = DONE_COLOR
        else:
            status_color = STATUS_BAD if model.status_text == "Try again" else TEXT_COLOR
        status_surface = small_font.render(model.status_text, True, status_color)
        screen.blit(status_surface, (500, 18))

        draw_generated_map(screen, draw_rect)
        screen.blit(overlay_surface, draw_rect.topleft)
        draw_completed_states(screen, draw_rect, model)

        for state_name, polygon in STATE_POLYGONS.items():
            if polygon:
                color = COMPLETE_COLORS.get(state_name, OUTLINE_COLOR) if state_name in model.completed_states else OUTLINE_COLOR
                draw_polygon_outline(screen, draw_rect, polygon, color, width=2)

        pygame.draw.rect(screen, (90, 90, 90), draw_rect, width=1)

        footer = tiny_font.render("Generated map base + traced polygon overlay | R reset | Esc quit", True, TEXT_COLOR)
        screen.blit(footer, (28, 444))

        done_text = tiny_font.render(
            f"Completed: {', '.join(sorted(model.completed_states)) if model.completed_states else 'none'}",
            True,
            TEXT_COLOR,
        )
        screen.blit(done_text, (28, 462))

        pygame.display.flip()
        clock.tick(60)

    pygame.quit()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

from __future__ import annotations

import importlib
import sys
from pathlib import Path
from typing import Dict, List, Tuple

import pygame
from PIL import Image

import generated_us_map as generated
import state_polygons


EDITOR_WINDOW = (1100, 900)
EDITOR_BG = (243, 241, 235)
TEXT_COLOR = (35, 35, 35)
INFO_COLOR = (70, 70, 70)
ACTIVE_COLOR = (205, 74, 59)
POINT_COLOR = (28, 72, 164)
PREVIEW_LABEL = (25, 25, 25)

MAIN_MAP_RECT = pygame.Rect(24, 54, 760, 520)
PREVIEW_MAP_RECT = pygame.Rect(40, 610, 480, 320)
PANEL_X = 820

STATE_KEYS = {
    pygame.K_1: 0,
    pygame.K_2: 1,
    pygame.K_3: 2,
    pygame.K_4: 3,
    pygame.K_5: 4,
    pygame.K_6: 5,
    pygame.K_7: 6,
    pygame.K_8: 7,
    pygame.K_9: 8,
    pygame.K_0: 9,
}

ROOT = Path(__file__).resolve().parents[5]
SOURCE_IMAGE = ROOT / "src" / "apps" / "assets" / "images" / "geography" / "usa-map-unselected.png"
STATE_POLYGON_FILE = Path(__file__).with_name("state_polygons.py")


def fit_rect(rect: pygame.Rect, grid_w: int, grid_h: int) -> pygame.Rect:
    cell_size = max(1, min(rect.width // grid_w, rect.height // grid_h))
    draw_w = grid_w * cell_size
    draw_h = grid_h * cell_size
    draw_x = rect.x + (rect.width - draw_w) // 2
    draw_y = rect.y + (rect.height - draw_h) // 2
    return pygame.Rect(draw_x, draw_y, draw_w, draw_h)


def cell_size_for_rect(draw_rect: pygame.Rect) -> Tuple[int, int]:
    return draw_rect.width // generated.GRID_WIDTH, draw_rect.height // generated.GRID_HEIGHT


def draw_generated_map(screen: pygame.Surface, draw_rect: pygame.Rect) -> None:
    cell_w, cell_h = cell_size_for_rect(draw_rect)

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
    temp_path = Path(__file__).with_name("_overlay_temp.png")
    crop.save(temp_path)
    try:
        surface = pygame.image.load(str(temp_path)).convert_alpha()
    finally:
        temp_path.unlink(missing_ok=True)
    return pygame.transform.smoothscale(surface, draw_rect.size)


def screen_to_grid(pos: Tuple[int, int], draw_rect: pygame.Rect) -> Tuple[int, int]:
    cell_w, cell_h = cell_size_for_rect(draw_rect)
    grid_x = int((pos[0] - draw_rect.x) / max(1, cell_w))
    grid_y = int((pos[1] - draw_rect.y) / max(1, cell_h))
    grid_x = max(0, min(generated.GRID_WIDTH - 1, grid_x))
    grid_y = max(0, min(generated.GRID_HEIGHT - 1, grid_y))
    return grid_x, grid_y


def grid_to_screen(point: Tuple[int, int], draw_rect: pygame.Rect) -> Tuple[int, int]:
    cell_w, cell_h = cell_size_for_rect(draw_rect)
    return (
        draw_rect.x + point[0] * cell_w + cell_w // 2,
        draw_rect.y + point[1] * cell_h + cell_h // 2,
    )


def export_polygons(polygons: Dict[str, List[Tuple[int, int]]]) -> None:
    lines = [
        "from __future__ import annotations",
        "",
        "from typing import Dict, List, Optional, Sequence, Tuple",
        "",
        "Point = Tuple[int, int]",
        "Polygon = List[Point]",
        "",
        "TRACE_STATES: Tuple[str, ...] = (",
    ]
    for state_name in state_polygons.TRACE_STATES:
        lines.append(f"    {state_name!r},")
    lines.extend(
        [
            ")",
            "",
            "STATE_POLYGONS: Dict[str, Polygon] = {",
        ]
    )
    for state_name in state_polygons.TRACE_STATES:
        lines.append(f"    {state_name!r}: {polygons.get(state_name, [])!r},")
    lines.extend(
        [
            "}",
            "",
            "",
            "def point_in_polygon(x: float, y: float, polygon: Sequence[Point]) -> bool:",
            "    if len(polygon) < 3:",
            "        return False",
            "",
            "    inside = False",
            "    j = len(polygon) - 1",
            "    for i in range(len(polygon)):",
            "        xi, yi = polygon[i]",
            "        xj, yj = polygon[j]",
            "        intersects = ((yi > y) != (yj > y)) and (",
            "            x < (xj - xi) * (y - yi) / ((yj - yi) or 1e-9) + xi",
            "        )",
            "        if intersects:",
            "            inside = not inside",
            "        j = i",
            "    return inside",
            "",
            "",
            "def get_state_at_grid_cell(grid_x: int, grid_y: int) -> Optional[str]:",
            "    for state_name, polygon in STATE_POLYGONS.items():",
            "        if point_in_polygon(grid_x + 0.5, grid_y + 0.5, polygon):",
            "            return state_name",
            "    return None",
            "",
        ]
    )
    STATE_POLYGON_FILE.write_text("\n".join(lines), encoding="utf-8")


def reload_polygons_module() -> None:
    importlib.reload(state_polygons)


def polygon_centroid(points: List[Tuple[int, int]]) -> Tuple[float, float]:
    if not points:
        return (0.0, 0.0)
    return (
        sum(point[0] for point in points) / len(points),
        sum(point[1] for point in points) / len(points),
    )


def main() -> int:
    pygame.init()
    pygame.display.set_caption("BitSlate State Polygon Editor")
    screen = pygame.display.set_mode(EDITOR_WINDOW)
    clock = pygame.time.Clock()

    font = pygame.font.SysFont("arial", 20)
    small_font = pygame.font.SysFont("arial", 15)
    tiny_font = pygame.font.SysFont("arial", 13)

    main_draw_rect = fit_rect(MAIN_MAP_RECT, generated.GRID_WIDTH, generated.GRID_HEIGHT)
    preview_draw_rect = fit_rect(PREVIEW_MAP_RECT, generated.GRID_WIDTH, generated.GRID_HEIGHT)
    overlay_surface = load_overlay_surface(main_draw_rect)

    polygons: Dict[str, List[Tuple[int, int]]] = {
        name: list(state_polygons.STATE_POLYGONS.get(name, []))
        for name in state_polygons.TRACE_STATES
    }

    trace_states = list(state_polygons.TRACE_STATES)
    current_index = 0
    current_state = trace_states[current_index]
    show_overlay = True
    overlay_alpha = 210
    status = "Ready"

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    running = False
                elif event.key in STATE_KEYS:
                    idx = STATE_KEYS[event.key]
                    if idx < len(trace_states):
                        current_index = idx
                    current_state = trace_states[current_index]
                    status = f"Selected {current_state}"
                elif event.key == pygame.K_n:
                    current_index = (current_index + 1) % len(trace_states)
                    current_state = trace_states[current_index]
                    status = f"Selected {current_state}"
                elif event.key == pygame.K_b:
                    current_index = (current_index - 1) % len(trace_states)
                    current_state = trace_states[current_index]
                    status = f"Selected {current_state}"
                elif event.key == pygame.K_BACKSPACE:
                    if polygons[current_state]:
                        polygons[current_state].pop()
                        status = "Removed last point"
                elif event.key == pygame.K_c:
                    polygons[current_state].clear()
                    status = f"Cleared {current_state}"
                elif event.key == pygame.K_s:
                    export_polygons(polygons)
                    reload_polygons_module()
                    saved_count = sum(1 for points in polygons.values() if points)
                    print(
                        f"Saved polygons: states_with_polygons={saved_count} "
                        f"selected_state={current_state} output={STATE_POLYGON_FILE}"
                    )
                    status = f"Saved {saved_count} states"
                elif event.key == pygame.K_o:
                    show_overlay = not show_overlay
                    status = f"Overlay {'on' if show_overlay else 'off'}"
                elif event.key == pygame.K_LEFTBRACKET:
                    overlay_alpha = max(0, overlay_alpha - 15)
                    status = f"Overlay alpha {overlay_alpha}"
                elif event.key == pygame.K_RIGHTBRACKET:
                    overlay_alpha = min(255, overlay_alpha + 15)
                    status = f"Overlay alpha {overlay_alpha}"
                elif event.key == pygame.K_p:
                    print(f"{current_state} =", polygons[current_state])
                    status = f"Printed {current_state}"
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                if main_draw_rect.collidepoint(event.pos):
                    point = screen_to_grid(event.pos, main_draw_rect)
                    polygons[current_state].append(point)
                    print(f"{current_state} add point {point}")
                    status = f"Added {point}"

        screen.fill(EDITOR_BG)

        title = font.render(
            f"State {current_index + 1}/{len(trace_states)}: {current_state}",
            True,
            TEXT_COLOR,
        )
        screen.blit(title, (24, 16))
        status_surface = small_font.render(status, True, INFO_COLOR)
        screen.blit(status_surface, (500, 20))
        alpha_surface = tiny_font.render(f"Overlay alpha: {overlay_alpha}", True, INFO_COLOR)
        screen.blit(alpha_surface, (820, 54))

        draw_generated_map(screen, main_draw_rect)
        if show_overlay:
            overlay_surface.set_alpha(overlay_alpha)
            screen.blit(overlay_surface, main_draw_rect.topleft)
        pygame.draw.rect(screen, (80, 80, 80), main_draw_rect, width=1)

        draw_generated_map(screen, preview_draw_rect)
        pygame.draw.rect(screen, (80, 80, 80), preview_draw_rect, width=1)

        for state_name, polygon in polygons.items():
            if not polygon:
                continue
            color = ACTIVE_COLOR if state_name == current_state else (86, 104, 164)
            main_points = [grid_to_screen(point, main_draw_rect) for point in polygon]
            preview_points = [grid_to_screen(point, preview_draw_rect) for point in polygon]
            if len(main_points) > 1:
                pygame.draw.lines(screen, color, False, main_points, width=2)
                pygame.draw.lines(screen, color, False, preview_points, width=2)
            for point in main_points:
                pygame.draw.circle(screen, POINT_COLOR, point, 4)
            for point in preview_points:
                pygame.draw.circle(screen, POINT_COLOR, point, 2)

            cx, cy = polygon_centroid(polygon)
            main_label = tiny_font.render(state_name, True, color)
            preview_label = tiny_font.render(state_name, True, PREVIEW_LABEL)
            screen.blit(main_label, grid_to_screen((int(cx), int(cy)), main_draw_rect))
            screen.blit(preview_label, grid_to_screen((int(cx), int(cy)), preview_draw_rect))

        mouse_pos = pygame.mouse.get_pos()
        if main_draw_rect.collidepoint(mouse_pos):
            grid_x, grid_y = screen_to_grid(mouse_pos, main_draw_rect)
            hover = tiny_font.render(f"Mouse grid: {grid_x}, {grid_y}", True, TEXT_COLOR)
            screen.blit(hover, (820, 460))

        controls = [
            "1-9,0 select state 1-10",
            "N next state",
            "B previous state",
            "Left click add point",
            "Backspace undo",
            "C clear current",
            "S save/export",
            "O toggle overlay",
            "[ ] overlay alpha",
            "P print polygon",
            "Esc quit",
        ]
        for index, line in enumerate(controls):
            screen.blit(small_font.render(line, True, TEXT_COLOR), (820, 90 + index * 26))

        list_title = small_font.render("Trace set", True, TEXT_COLOR)
        screen.blit(list_title, (820, 390))
        for idx, state_name in enumerate(trace_states):
            prefix = ">" if idx == current_index else " "
            marker = "*" if polygons.get(state_name) else "-"
            line = f"{prefix} {idx + 1:02d} {marker} {state_name}"
            color = ACTIVE_COLOR if idx == current_index else TEXT_COLOR
            screen.blit(tiny_font.render(line, True, color), (820, 418 + idx * 18))

        preview_title = small_font.render("480x320 Preview", True, TEXT_COLOR)
        screen.blit(preview_title, (40, 586))

        footer = tiny_font.render(
            "Saved points use generated-map grid coordinates, not screen pixels.",
            True,
            INFO_COLOR,
        )
        screen.blit(footer, (820, 500))

        pygame.display.flip()
        clock.tick(60)

    pygame.quit()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

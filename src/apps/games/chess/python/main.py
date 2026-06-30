from __future__ import annotations

import sys
from pathlib import Path
from typing import Dict, Optional, Tuple

import pygame

from chess_model import ChessModel, Piece, WHITE


WINDOW_W = 480
WINDOW_H = 320
BOARD_SIZE = 304
BOARD_X = 8
BOARD_Y = 8
SQUARE_SIZE = BOARD_SIZE // 8
PANEL_X = 324
PANEL_W = 148
PANEL_Y = 10

BG_COLOR = (225, 229, 232)
PANEL_COLOR = (243, 245, 247)
PANEL_BORDER = (88, 96, 105)
TEXT_COLOR = (26, 26, 26)
HILITE_COLOR = (49, 115, 255)
MOVE_COLOR = (34, 170, 90)
RESET_COLOR = (207, 79, 79)
RESET_BORDER = (133, 34, 34)

ASSET_DIR = Path(__file__).resolve().parents[4] / "assets" / "images" / "chess-piece-png"

PIECE_FILES = {
    "white_pawn": "pawn_white.png",
    "white_rook": "rook_white.png",
    "white_knight": "knight_white.png",
    "white_bishop": "bishop_white.png",
    "white_queen": "queen_white.png",
    "white_king": "king_white.png",
    "black_pawn": "pawn_black.png",
    "black_rook": "rook_black.png",
    "black_knight": "knight_black.png",
    "black_bishop": "bishop_black.png",
    "black_queen": "queen_black.png",
    "black_king": "king_black.png",
}


def wrap_text(text: str, font: pygame.font.Font, max_width: int) -> list[str]:
    words = text.split()
    if not words:
        return [""]

    lines: list[str] = []
    current = words[0]
    for word in words[1:]:
        test = current + " " + word
        if font.size(test)[0] <= max_width:
            current = test
        else:
            lines.append(current)
            current = word
    lines.append(current)
    return lines


def board_coords_from_mouse(pos: Tuple[int, int]) -> Optional[Tuple[int, int]]:
    x, y = pos
    if not (BOARD_X <= x < BOARD_X + BOARD_SIZE and BOARD_Y <= y < BOARD_Y + BOARD_SIZE):
        return None
    col = (x - BOARD_X) // SQUARE_SIZE
    row = (y - BOARD_Y) // SQUARE_SIZE
    return int(row), int(col)


def scale_piece_image(surface: pygame.Surface) -> pygame.Surface:
    size = int(SQUARE_SIZE * 0.86)
    return pygame.transform.smoothscale(surface, (size, size))


def piece_key(piece: Piece) -> str:
    return f"{piece.color}_{piece.kind}"


def selected_piece_label(model: ChessModel) -> str:
    if model.selected is None:
        return "None"
    row, col = model.selected
    piece = model.get_piece(row, col)
    if piece is None:
        return "None"
    file_rank = f"{chr(ord('A') + col)}{8 - row}"
    return f"{piece.display_name()} @ {file_rank}"


def draw_panel(
    screen: pygame.Surface,
    fonts: Dict[str, pygame.font.Font],
    model: ChessModel,
    reset_rect: pygame.Rect,
) -> None:
    panel_rect = pygame.Rect(PANEL_X, PANEL_Y, PANEL_W, 300)
    pygame.draw.rect(screen, PANEL_COLOR, panel_rect, border_radius=8)
    pygame.draw.rect(screen, PANEL_BORDER, panel_rect, width=2, border_radius=8)

    title = fonts["title"].render("Chess", True, TEXT_COLOR)
    screen.blit(title, (PANEL_X + 16, PANEL_Y + 12))

    lines = [
        ("Turn", model.current_turn.title()),
        ("Selected", selected_piece_label(model)),
    ]

    y = PANEL_Y + 48
    for label, value in lines:
        label_surf = fonts["label"].render(label, True, (70, 76, 82))
        screen.blit(label_surf, (PANEL_X + 12, y))
        y += 18
        value_lines = wrap_text(value, fonts["body"], PANEL_W - 24)
        for line in value_lines:
            text_surf = fonts["body"].render(line, True, TEXT_COLOR)
            screen.blit(text_surf, (PANEL_X + 12, y))
            y += 16
        y += 8

    status_label = fonts["label"].render("Status", True, (70, 76, 82))
    screen.blit(status_label, (PANEL_X + 12, y))
    y += 18
    for line in wrap_text(model.last_message, fonts["body"], PANEL_W - 24):
        text_surf = fonts["body"].render(line, True, TEXT_COLOR)
        screen.blit(text_surf, (PANEL_X + 12, y))
        y += 16

    pygame.draw.rect(screen, RESET_COLOR, reset_rect, border_radius=8)
    pygame.draw.rect(screen, RESET_BORDER, reset_rect, width=2, border_radius=8)
    reset_text = fonts["button"].render("Reset", True, (250, 250, 250))
    screen.blit(reset_text, reset_text.get_rect(center=reset_rect.center))


def draw_board(
    screen: pygame.Surface,
    board_image: pygame.Surface,
    piece_images: Dict[str, pygame.Surface],
    model: ChessModel,
) -> None:
    screen.blit(board_image, (BOARD_X, BOARD_Y))

    if model.selected is not None:
        row, col = model.selected
        rect = pygame.Rect(BOARD_X + col * SQUARE_SIZE, BOARD_Y + row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE)
        pygame.draw.rect(screen, HILITE_COLOR, rect, width=3)
        for move_row, move_col in model.legal_moves(row, col):
            move_rect = pygame.Rect(
                BOARD_X + move_col * SQUARE_SIZE + SQUARE_SIZE // 2 - 6,
                BOARD_Y + move_row * SQUARE_SIZE + SQUARE_SIZE // 2 - 6,
                12,
                12,
            )
            pygame.draw.ellipse(screen, MOVE_COLOR, move_rect)

    for row in range(8):
        for col in range(8):
            piece = model.get_piece(row, col)
            if piece is None:
                continue
            image = piece_images[piece_key(piece)]
            x = BOARD_X + col * SQUARE_SIZE + (SQUARE_SIZE - image.get_width()) // 2
            y = BOARD_Y + row * SQUARE_SIZE + (SQUARE_SIZE - image.get_height()) // 2
            screen.blit(image, (x, y))


def load_assets() -> Tuple[pygame.Surface, Dict[str, pygame.Surface]]:
    board_path = ASSET_DIR / "blank-board.png"
    board = pygame.image.load(str(board_path)).convert_alpha()
    board = pygame.transform.smoothscale(board, (BOARD_SIZE, BOARD_SIZE))

    piece_images: Dict[str, pygame.Surface] = {}
    for key, filename in PIECE_FILES.items():
        image_path = ASSET_DIR / filename
        piece_images[key] = scale_piece_image(pygame.image.load(str(image_path)).convert_alpha())
    return board, piece_images


def main() -> None:
    pygame.init()
    pygame.display.set_caption("BitSlate Chess")

    screen = pygame.display.set_mode((WINDOW_W, WINDOW_H))
    clock = pygame.time.Clock()

    fonts = {
        "title": pygame.font.SysFont("arial", 28, bold=True),
        "label": pygame.font.SysFont("arial", 15, bold=True),
        "body": pygame.font.SysFont("arial", 16),
        "button": pygame.font.SysFont("arial", 20, bold=True),
    }

    board_image, piece_images = load_assets()
    model = ChessModel()

    reset_rect = pygame.Rect(PANEL_X + 22, 266, 104, 34)

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                running = False
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                mouse_pos = event.pos
                if reset_rect.collidepoint(mouse_pos):
                    model.reset()
                    continue

                coords = board_coords_from_mouse(mouse_pos)
                if coords is None:
                    continue

                row, col = coords
                clicked_piece = model.get_piece(row, col)

                if model.selected is None:
                    model.select_piece(row, col)
                    continue

                from_row, from_col = model.selected
                if (row, col) == (from_row, from_col):
                    model.selected = None
                    model.last_message = "Selection cleared"
                    continue

                if model.move_piece(from_row, from_col, row, col):
                    continue

                if clicked_piece is not None and clicked_piece.color == model.current_turn:
                    model.select_piece(row, col)

        screen.fill(BG_COLOR)
        draw_board(screen, board_image, piece_images, model)
        draw_panel(screen, fonts, model, reset_rect)
        pygame.display.flip()
        clock.tick(60)

    pygame.quit()


if __name__ == "__main__":
    try:
        main()
    except FileNotFoundError as exc:
        print(f"Missing asset: {exc}")
        sys.exit(1)

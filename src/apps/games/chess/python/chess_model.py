from __future__ import annotations

from dataclasses import dataclass
from typing import List, Optional, Tuple


BOARD_SIZE = 8
WHITE = "white"
BLACK = "black"


@dataclass(frozen=True)
class Piece:
    color: str
    kind: str

    def short_name(self) -> str:
        return f"{self.color[0]}{self.kind}"

    def display_name(self) -> str:
        return f"{self.color.title()} {self.kind.title()}"


Move = Tuple[int, int]


class ChessModel:
    def __init__(self) -> None:
        self.board: List[List[Optional[Piece]]] = []
        self.current_turn = WHITE
        self.selected: Optional[Move] = None
        self.last_message = "White to move"
        self.reset()

    def reset(self) -> None:
        self.board = [[None for _ in range(BOARD_SIZE)] for _ in range(BOARD_SIZE)]
        self.current_turn = WHITE
        self.selected = None
        self.last_message = "White to move"

        back_rank = ["rook", "knight", "bishop", "queen", "king", "bishop", "knight", "rook"]
        for col, kind in enumerate(back_rank):
            self.board[0][col] = Piece(BLACK, kind)
            self.board[1][col] = Piece(BLACK, "pawn")
            self.board[6][col] = Piece(WHITE, "pawn")
            self.board[7][col] = Piece(WHITE, kind)

    def in_bounds(self, row: int, col: int) -> bool:
        return 0 <= row < BOARD_SIZE and 0 <= col < BOARD_SIZE

    def get_piece(self, row: int, col: int) -> Optional[Piece]:
        if not self.in_bounds(row, col):
            return None
        return self.board[row][col]

    def select_piece(self, row: int, col: int) -> bool:
        piece = self.get_piece(row, col)
        if piece is None:
            self.last_message = "Select a piece"
            return False
        if piece.color != self.current_turn:
            self.last_message = f"{self.current_turn.title()} to move"
            return False
        self.selected = (row, col)
        self.last_message = f"Selected {piece.display_name()}"
        return True

    def legal_moves(self, row: int, col: int) -> List[Move]:
        piece = self.get_piece(row, col)
        if piece is None:
            return []
        return self._legal_moves_for_piece(row, col, piece, enforce_turn=False)

    def move_piece(self, from_row: int, from_col: int, to_row: int, to_col: int) -> bool:
        if not self.in_bounds(from_row, from_col) or not self.in_bounds(to_row, to_col):
            self.last_message = "Move out of bounds"
            return False

        piece = self.get_piece(from_row, from_col)
        if piece is None:
            self.last_message = "No piece selected"
            return False
        if piece.color != self.current_turn:
            self.last_message = f"{self.current_turn.title()} to move"
            return False

        legal = self._legal_moves_for_piece(from_row, from_col, piece, enforce_turn=False)
        if (to_row, to_col) not in legal:
            self.last_message = "Illegal move"
            return False

        target = self.get_piece(to_row, to_col)
        self.board[to_row][to_col] = piece
        self.board[from_row][from_col] = None

        if piece.kind == "pawn" and (to_row == 0 or to_row == BOARD_SIZE - 1):
            self.board[to_row][to_col] = Piece(piece.color, "queen")
            piece = self.board[to_row][to_col]

        self.current_turn = BLACK if self.current_turn == WHITE else WHITE
        self.selected = None

        if target is not None:
            self.last_message = f"{piece.display_name()} captured {target.display_name()}"
        else:
            self.last_message = f"{piece.display_name()} moved"
        return True

    def _legal_moves_for_piece(
        self, row: int, col: int, piece: Piece, enforce_turn: bool = False
    ) -> List[Move]:
        if enforce_turn and piece.color != self.current_turn:
            return []

        if piece.kind == "pawn":
            return self._pawn_moves(row, col, piece)
        if piece.kind == "rook":
            return self._ray_moves(row, col, piece, [(-1, 0), (1, 0), (0, -1), (0, 1)])
        if piece.kind == "bishop":
            return self._ray_moves(row, col, piece, [(-1, -1), (-1, 1), (1, -1), (1, 1)])
        if piece.kind == "queen":
            return self._ray_moves(
                row,
                col,
                piece,
                [(-1, 0), (1, 0), (0, -1), (0, 1), (-1, -1), (-1, 1), (1, -1), (1, 1)],
            )
        if piece.kind == "knight":
            return self._step_moves(
                row,
                col,
                piece,
                [(-2, -1), (-2, 1), (-1, -2), (-1, 2), (1, -2), (1, 2), (2, -1), (2, 1)],
            )
        if piece.kind == "king":
            return self._step_moves(
                row,
                col,
                piece,
                [(-1, -1), (-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 0), (1, 1)],
            )
        return []

    def _pawn_moves(self, row: int, col: int, piece: Piece) -> List[Move]:
        moves: List[Move] = []
        direction = -1 if piece.color == WHITE else 1
        start_row = 6 if piece.color == WHITE else 1

        one_row = row + direction
        if self.in_bounds(one_row, col) and self.get_piece(one_row, col) is None:
            moves.append((one_row, col))
            two_row = row + 2 * direction
            if row == start_row and self.in_bounds(two_row, col) and self.get_piece(two_row, col) is None:
                moves.append((two_row, col))

        for delta_col in (-1, 1):
            capture_col = col + delta_col
            capture_row = row + direction
            if not self.in_bounds(capture_row, capture_col):
                continue
            target = self.get_piece(capture_row, capture_col)
            if target is not None and target.color != piece.color:
                moves.append((capture_row, capture_col))

        return moves

    def _ray_moves(self, row: int, col: int, piece: Piece, directions: List[Move]) -> List[Move]:
        moves: List[Move] = []
        for d_row, d_col in directions:
            cur_row = row + d_row
            cur_col = col + d_col
            while self.in_bounds(cur_row, cur_col):
                target = self.get_piece(cur_row, cur_col)
                if target is None:
                    moves.append((cur_row, cur_col))
                else:
                    if target.color != piece.color:
                        moves.append((cur_row, cur_col))
                    break
                cur_row += d_row
                cur_col += d_col
        return moves

    def _step_moves(self, row: int, col: int, piece: Piece, deltas: List[Move]) -> List[Move]:
        moves: List[Move] = []
        for d_row, d_col in deltas:
            new_row = row + d_row
            new_col = col + d_col
            if not self.in_bounds(new_row, new_col):
                continue
            target = self.get_piece(new_row, new_col)
            if target is None or target.color != piece.color:
                moves.append((new_row, new_col))
        return moves

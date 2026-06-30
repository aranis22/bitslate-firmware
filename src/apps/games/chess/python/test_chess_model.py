from chess_model import ChessModel, WHITE, BLACK


def test_initial_board_setup() -> None:
    model = ChessModel()
    assert model.get_piece(7, 4).kind == "king"
    assert model.get_piece(7, 4).color == WHITE
    assert model.get_piece(0, 4).kind == "king"
    assert model.get_piece(0, 4).color == BLACK
    assert model.get_piece(6, 0).kind == "pawn"
    assert model.get_piece(1, 7).kind == "pawn"


def test_white_moves_first() -> None:
    model = ChessModel()
    assert model.current_turn == WHITE


def test_pawn_single_move() -> None:
    model = ChessModel()
    assert model.move_piece(6, 4, 5, 4) is True
    assert model.get_piece(5, 4) is not None
    assert model.get_piece(6, 4) is None


def test_pawn_double_move_from_start() -> None:
    model = ChessModel()
    moves = model.legal_moves(6, 4)
    assert (4, 4) in moves
    assert model.move_piece(6, 4, 4, 4) is True


def test_illegal_pawn_move_rejected() -> None:
    model = ChessModel()
    assert model.move_piece(6, 4, 3, 4) is False


def test_knight_move_works() -> None:
    model = ChessModel()
    assert model.move_piece(7, 1, 5, 2) is True
    moved_piece = model.get_piece(5, 2)
    assert moved_piece is not None
    assert moved_piece.kind == "knight"


def test_rook_blocked_by_pawn() -> None:
    model = ChessModel()
    moves = model.legal_moves(7, 0)
    assert moves == []
    assert model.move_piece(7, 0, 5, 0) is False


def test_turn_alternates_after_valid_move() -> None:
    model = ChessModel()
    assert model.move_piece(6, 4, 4, 4) is True
    assert model.current_turn == BLACK


def test_cannot_move_opponent_piece_on_wrong_turn() -> None:
    model = ChessModel()
    assert model.move_piece(1, 4, 3, 4) is False
    assert model.current_turn == WHITE


def test_reset_works() -> None:
    model = ChessModel()
    assert model.move_piece(6, 4, 4, 4) is True
    model.reset()
    assert model.current_turn == WHITE
    assert model.get_piece(6, 4) is not None
    assert model.get_piece(4, 4) is None


def run_all() -> None:
    test_initial_board_setup()
    test_white_moves_first()
    test_pawn_single_move()
    test_pawn_double_move_from_start()
    test_illegal_pawn_move_rejected()
    test_knight_move_works()
    test_rook_blocked_by_pawn()
    test_turn_alternates_after_valid_move()
    test_cannot_move_opponent_piece_on_wrong_turn()
    test_reset_works()
    print("Chess model tests passed.")


if __name__ == "__main__":
    run_all()

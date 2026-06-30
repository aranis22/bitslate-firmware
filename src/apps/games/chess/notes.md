# Chess Notes

## Current checkpoints

- Python desktop prototype: working
- LVGL asset conversion: working
- Embedded LVGL Chess v0: working

## Embedded LVGL layout

- Screen: `480x320`
- Board size: `304x304`
- Board position: centered
- Square size: `38x38`
- Piece bitmap size: `24x24`
- Squares are individually clickable LVGL objects
- Pieces render as LVGL ARGB8888 image descriptors above the squares

## Assets

Uses existing PNGs in:

`src/assets/images/chess-piece-png/`

Files used:

- `blank-board.png`
- `pawn_white.png`
- `rook_white.png`
- `knight_white.png`
- `bishop_white.png`
- `queen_white.png`
- `king_white.png`
- `pawn_black.png`
- `rook_black.png`
- `knight_black.png`
- `bishop_black.png`
- `queen_black.png`
- `king_black.png`

Generated embedded assets:

- `src/assets/images/chess/generated/chess_piece_assets.h`
- `src/assets/images/chess/generated/chess_piece_assets.c`

## Model boundaries

Desktop model:

- `python/chess_model.py` stays pygame-free.

Embedded model:

- `lvgl/ChessGameModel.*` stays LVGL-free and owns the chess state/rules.

Responsibilities:

- board state
- move validation
- legal move generation
- turn tracking
- reset

Embedded v0 movement rules:

- pawns
- rooks
- knights
- bishops
- queens
- kings
- captures
- own-piece capture prevention
- sliding-piece blocking
- pawn first double move
- pawn diagonal capture
- auto-promote to queen

Not implemented yet:

- check/checkmate
- castling
- en passant
- stalemate
- timers
- AI
- multiplayer

## Next steps

1. Chess v1: Human vs AI
2. simple rule-based AI first
3. random legal move or simple material-priority move
4. later ESP-NOW BitSlate-to-BitSlate multiplayer

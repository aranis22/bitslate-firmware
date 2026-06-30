# BitSlate Chess

This folder holds the BitSlate Chess app work across the desktop prototype and the current embedded LVGL checkpoint.

Current state:

- Python desktop prototype: working
- LVGL chess asset conversion: working
- Embedded LVGL board render: working
- Tap-to-move playable Chess v0: working
- Located under `src/apps/games/chess/`
- Uses existing chess PNG assets from `src/assets/images/chess-piece-png/`

Current embedded board checkpoint:

- 24x24 LVGL piece bitmaps
- 304x304 centered board
- 38x38 clickable squares
- Board and pieces render on hardware
- Tap own piece to select
- Tap legal destination to move
- Turn-based play with embedded move validation

Implemented in v0:

- Standard setup
- White moves first
- Turn alternation
- Legal moves for pawns, rooks, knights, bishops, queens, and kings
- Captures
- Own-piece capture prevention
- Sliding-piece blocking
- Pawn double move from starting rank
- Pawn diagonal capture
- Auto-promote pawn to queen

Not implemented yet:

- Check/checkmate detection
- Castling
- En passant
- Stalemate
- Timers
- AI opponent
- Multiplayer/network play

Run locally:

```powershell
python src/apps/games/chess/python/test_chess_model.py
python -m py_compile src/apps/games/chess/python/main.py src/apps/games/chess/python/chess_model.py src/apps/games/chess/python/test_chess_model.py
python src/apps/games/chess/python/main.py
```

Next planned step:

- Chess v1: Human vs AI
  - simple rule-based AI first
  - random legal move or basic material-priority move
  - no deep engine yet

Later TODO:

- Chess multiplayer over ESP-NOW
  - one BitSlate as White, one as Black
  - sync moves wirelessly
  - handle reset/rematch

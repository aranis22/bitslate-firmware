# Flags Quiz Notes

## Current Checkpoint

- LVGL Flags Quiz prototype runs as the current BitSlate launch target.
- Current flags: Albania, Brazil, India, Nepal, Portugal, Seychelles.
- Generated LVGL RGB565 assets live in `generated/FlagsQuizAssets.h` and `generated/FlagsQuizAssets.cpp`.
- Source flag PNGs are kept in this app folder and converted by `python/convert_flag_assets.py`.
- UI shows one flag in a framed card and four large answer tiles in a 2x2 layout.
- Wrong answers show `Try Again`.
- Correct answers show bright green `Correct!`, disable input, then advance after a non-blocking 4-second LVGL timer.

## Known Limitations

- Fixed deterministic question order.
- No score screen yet.
- No launcher integration yet.

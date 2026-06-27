# Build Notes

## 2026-06-27 Checkpoint

- LovyanGFX display test works on the ESP32-S3 with the confirmed ILI9488 pinout.
- Touch works with the XPT2046-style resistive touch controller on shared SPI.
- Touch X mirroring is corrected inside `LGFX_BitSlate::getTouch(...)`.
- LVGL is not yet re-enabled.
- Next step: re-enable LVGL carefully after this GitHub checkpoint.

## 2026-06-27 LVGL Touch Checkpoint

- LVGL touch confirmation works on hardware.
- Button changes from `Touch me` to `Touched!`.
- Status label changes to green `Touch OK`.
- This confirms the LovyanGFX-to-LVGL touch bridge works.
- LVGL display flush works.
- Next step: build the real BitSlate home screen / launcher UI.
- Do not change verified display pins.

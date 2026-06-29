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

## 2026-06-27 Circuit Lab Early Prototype

- Simple Circuit Lab / STEM app screen exists.
- Wire placement is partially working.
- Wire placement behavior is weird but interesting.
- Current limitation: cannot drag wire directly from the left tray as expected.
- Frozen for now.
- Next direction is PhET/source research and desktop prototype workflow before continuing embedded simulation logic.

## 2026-06-28 PhET Research Workflow

- PhET reference repositories were cloned into ignored `research/phet-reference/`.
- BitSlate research notes live in `research/phet-notes/`.
- Complex STEM simulation workflow is now desktop prototype first, then pure logic, then C++ port, then LVGL integration.
- Circuit Lab should continue from a small desktop model before returning to embedded UI details.

## 2026-06-28 Desktop Circuit Lab Prototype

- Started a Python/pygame desktop prototype for BitSlate Circuit Lab.
- Prototype is intentionally separate from embedded LVGL firmware.
- Initial model uses components, terminals, wires, and simplified graph/path closed-loop detection.
- First goal is battery + bulb + wire logic before any C++ or LVGL integration.

## 2026-06-28 Circuit Lab Portable Core

- Desktop prototype worked visually with battery, bulb, wires, closed-loop detection, and bulb ON state.
- Python model was refactored into a portable shape using component types, terminals, wires, and graph/path logic.
- C++ portable core was created in `portable-core/circuit-lab/`.
- C++ core mirrors the Python graph/path logic.
- Embedded LVGL integration is next, but not done in this checkpoint.

## 2026-06-29 STEM Apps Roadmap

- Embedded Circuit Lab prototype was checkpointed on `circuit-lab-embedded-core`.
- Circuit Lab is now tracked as the first modular STEM app.
- Collision Lab v0 is the next planned STEM app.
- Workflow remains desktop prototype, portable model/core, embedded LVGL app, then polish.

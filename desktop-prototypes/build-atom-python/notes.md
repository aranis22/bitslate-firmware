# Build an Atom Prototype Notes

## Decoded PhET Concepts

- Proton count determines element identity and atomic number.
- Proton count plus neutron count determines mass number.
- Proton count minus electron count determines net charge.
- Electrons fill a simple first shell of 2 and second shell of 8 for the first two periodic-table rows.
- PhET intentionally allows unrealistic atoms so students can explore particle counts without simulation lockouts.

## Simplified v0 Plan

- Start with first 10 elements.
- Build a 480x320 pygame prototype.
- Use a left tray for proton, neutron, and electron.
- Use a central atom workspace with nucleus and shell rings.
- Show element name, symbol, atomic number, mass number, and charge.
- Add reset.
- Add simple challenge prompts later.

## Intended Model

- `protons`
- `neutrons`
- `electrons`
- `atomic_number()`
- `mass_number()`
- `charge()`
- `element_symbol()`
- `element_name()`

This model should later become the basis for a portable C++ core before embedded LVGL work begins.

## Portable C++ Core

- Added in `portable-core/build-atom/`.
- Mirrors the prototype's first-pass particle count logic.
- Keeps Arduino, LVGL, LovyanGFX, and pygame out of the model.
- Uses fixed lookup tables and no dynamic allocation.
- Supports first 10 elements plus unknown atom state.
- Ready to become the source of truth for the later embedded LVGL app.

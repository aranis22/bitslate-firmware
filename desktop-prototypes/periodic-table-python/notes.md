# Periodic Table Prototype Notes

## Screen Constraint

- Target display is 480x320 landscape.
- The UI must fit without scrolling for the first prototype.
- Touch targets should be large enough for finger/stylus use later on BitSlate.

## Planned Layout

- Left info panel: compact selected-element details.
- Right side: compressed periodic table grid.
- Tap/click an element cell to update the left panel.

## Lean Info Panel Fields

1. Atomic mass
2. Category / series
3. Group / period
4. Electron shells
5. State at room temp
6. Electronegativity

Unknown values should display as `-`.

## Deliberate Omissions For v0

- No temperature slider.
- No huge property list.
- No animations.
- No embedded LVGL integration yet.


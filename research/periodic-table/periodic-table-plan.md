# Periodic Table Plan

## App Goal

Create a BitSlate STEM app named `Periodic Table` that lets a learner tap an element and immediately see useful element properties in a compact info panel.

This is not a PhET lab.

## Reference

The visual and interaction inspiration is ptable.com Properties mode:

- Select an element.
- Show useful property data.
- Keep the table itself visible.

BitSlate should simplify this for a 480x320 offline classroom device.

## 480x320 Layout

- Left panel: selected element information.
- Right panel: compressed periodic table grid.
- Title can be small or omitted if the layout is too tight.
- The grid should prioritize recognizable periodic-table shape over full desktop detail.

## Chosen Left-Panel Fields

1. Atomic mass
2. Category / series
3. Group / period
4. Electron shells
5. State at room temp
6. Electronegativity

For unknown values, display `-`.

## Data Model

Each element record should eventually include:

- `atomic_number`
- `symbol`
- `name`
- `atomic_mass`
- `category`
- `group`
- `period`
- `grid_col`
- `grid_row`
- `shells`
- `state`
- `electronegativity`

## Planned Python v0 Behavior

- 480x320 pygame window.
- Left info panel starts with a placeholder prompt.
- Right compressed periodic table grid shows element symbols.
- User clicks an element cell.
- Left panel updates with symbol, name, and the six selected fields.
- Missing values display as `-`.

## Planned C++ Core

- Fixed-size element data table.
- Lookup by atomic number.
- Lookup by periodic-grid coordinates.
- No dynamic allocation.
- No LVGL dependency.
- No Arduino dependency.

## Planned LVGL App

- Reuse the same data model shape as the portable C++ core.
- Use LVGL buttons/objects for element cells.
- Keep the left info panel readable.
- Do not add temperature sliders, animations, or large property tables in v0.


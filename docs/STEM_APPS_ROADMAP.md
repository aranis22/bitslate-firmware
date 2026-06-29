# BitSlate STEM Apps Roadmap

## Current completed/prototype apps

### STEM subject grouping

```text
STEM Apps
  Physics
    - Circuit Lab
    - Collision Lab
    - Masses & Springs
  Chemistry
    - Build an Atom
    - Periodic Table
  Math
    - empty/planned next
```

### Circuit Lab v0

Status:

- Desktop Python prototype: working
- Portable C++ core: created
- Embedded LVGL version: working/checkpointed
- Current features:
  - Battery placement
  - Bulb placement
  - Wire terminal selection
  - Closed-loop detection
  - Bulb ON/OFF
- Known limitations:
  - Not polished
  - No dragging
  - No resistor/switch/current animation yet
  - Graph/path logic only, not full circuit solver

### Collision Lab v0

Status:

- Desktop Python prototype: working
- Portable C++ core: created
- Embedded LVGL version: working/checkpointed
- Current features:
  - Two balls
  - Mass sliders
  - Velocity sliders
  - Play/pause/reset
  - 1D elastic collision
  - Wall bounce
  - Momentum and kinetic energy display
- Known limitations:
  - No 2D angles
  - No inelasticity slider
  - No graph view

### Masses & Springs v0

Status:

- Desktop Python prototype: working
- Portable C++ core: created
- Embedded LVGL version: working/checkpointed
- Current features:
  - One spring
  - One mass
  - Hooke's law
  - Gravity
  - Damping
  - Play/pause/reset
  - Mass, k, damping, gravity sliders
  - Displacement/force readouts
- Known limitations:
  - No multiple springs
  - No energy graph
  - No period measurement
  - No ruler/tooling
  - No BitSlate OS launcher integration yet

### Build an Atom v0

Status:

- Desktop Python prototype: working
- Portable C++ core: created
- Embedded LVGL version: working/checkpointed
- Current features:
  - Proton, neutron, and electron counts
  - Simple element identity
  - Atomic number, mass number, and charge readouts
  - Nucleus and electron shell workspace
  - Reset
- Known limitations:
  - No drag mode
  - No game/challenge mode
  - No BitSlate OS launcher integration yet

### Periodic Table v0

Status:

- Desktop Python prototype: working
- Portable C++ core: created
- Embedded LVGL version: working/checkpointed

Current features:

- Interactive periodic table inspired by ptable-style properties mode
- 480x320 landscape layout
- Left info panel with lean element details
- Right compressed periodic table grid
- Clickable periodic grid
- Category colors
- Symbol-only cells for readability

Chosen left-panel fields:

- Atomic mass
- Category / series
- Group / period
- Electron shells
- State at room temp
- Electronegativity

Known limitations:

- No zoom/scroll yet
- No temperature slider
- No advanced property pages
- Still using a temporary narrow PlatformIO build filter for validation

## Next app: Math

Status:

- Folder reserved
- No prototype selected yet

## Skipped for now

- Forces & Motion
- Wave Interference

## Workflow rule

For complex STEM apps:

1. Desktop prototype
2. Portable model/core
3. Embedded LVGL app
4. Then polish

Do not debug physics logic + embedded UI at the same time.

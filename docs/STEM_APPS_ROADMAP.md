# BitSlate STEM Apps Roadmap

## Current completed/prototype apps

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

## Next app: Masses & Springs / Hooke's Law

Goal:

- Desktop prototype first
- One spring and one mass at first
- Vertical motion only
- Spring constant slider
- Mass slider
- Damping slider
- Gravity control
- Play/pause/reset
- Drag mass down and release

Future:

- Embedded LVGL port
- Energy display
- Equilibrium marker
- Measurement tools

## Skipped for now

- Forces & Motion
- Wave Interference
- Embedded LVGL port

## Workflow rule

For complex STEM apps:

1. Desktop prototype
2. Portable model/core
3. Embedded LVGL app
4. Then polish

Do not debug physics logic + embedded UI at the same time.

# BitSlate STEM Apps Roadmap

## Current completed/prototype apps

### Circuit Lab

Status:

- Desktop Python prototype: working
- Portable C++ core: created
- Embedded LVGL version: early prototype working
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

## Next app: Collision Lab v0

Goal:

- Desktop prototype first
- 1D collision only at first
- Two balls
- Mass sliders
- Velocity sliders
- Play/pause/reset
- Elastic collision
- Wall bounce optional

Future:

- 2D direction
- Angle controls
- Momentum arrows
- Kinetic energy display
- Inelastic collision slider
- Embedded LVGL port

## Workflow rule

For complex STEM apps:

1. Desktop prototype
2. Portable model/core
3. Embedded LVGL app
4. Then polish

Do not debug physics logic + embedded UI at the same time.

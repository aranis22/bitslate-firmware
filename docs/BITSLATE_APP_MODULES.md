# BitSlate App Modules

## Purpose

BitSlate OS will eventually launch apps individually instead of hardcoding one app in `main.cpp`.

## Current STEM apps

### Circuit Lab v0

Status:

- Embedded LVGL version working
- Desktop prototype exists
- Portable C++ core exists
- Features:
  - battery
  - bulb
  - wire connections
  - closed-loop detection
  - bulb ON/OFF
- Current limitations:
  - no full circuit solver
  - no resistor/switch/current animation
  - no polished drag-from-tray behavior

Expected app entry:
`CircuitLabApp::create()`

### Collision Lab v0

Status:

- Desktop prototype exists
- Portable C++ core exists
- Embedded LVGL version working
- Features:
  - two balls
  - mass sliders
  - velocity sliders
  - play/pause/reset
  - 1D elastic collision
  - wall bounce
  - momentum and kinetic energy display
- Current limitations:
  - no 2D angles
  - no inelasticity slider
  - no graph view

Expected app entry:
`CollisionLabApp::create()`

### Masses & Springs v0

Status:

- Desktop Python prototype: working
- Portable C++ core: created
- Embedded LVGL version: working
- Features:
  - one spring
  - one mass
  - Hooke's law
  - gravity
  - damping
  - play/pause/reset
  - mass, k, damping, gravity sliders
  - displacement/force readouts
- Current limitations:
  - no multiple springs
  - no energy graph
  - no period measurement
  - no ruler/tooling
  - no BitSlate OS launcher integration yet

Expected app entry:
`MassesSpringsApp::create()`

### Build an Atom v0

Status:

- Desktop Python prototype: working
- Portable C++ core: created
- Embedded LVGL version: working
- Features:
  - proton count
  - neutron count
  - electron count
  - element identity
  - atomic number
  - mass number
  - charge
  - right-side element card
- Current limitations:
  - no drag mode
  - no game/challenge mode
  - no BitSlate OS launcher integration yet

Expected app entry:
`BuildAtomApp::create()`

### Periodic Table

Status:

- Research/planning started
- Python prototype not implemented yet
- C++ core not implemented yet
- Embedded LVGL not implemented yet

Planned features:

- 480x320 landscape layout
- left info panel
- compressed periodic table grid
- tap/click an element to update details
- top six fields:
  - atomic mass
  - category / series
  - group / period
  - electron shells
  - state at room temp
  - electronegativity

Expected app entry:
`PeriodicTableApp::create()`

## Future OS launcher rule

For now, `main.cpp` can temporarily call one app directly.

Later, BitSlate OS should expose a launcher menu:

- STEM Apps
  - Circuit Lab
  - Collision Lab
  - Masses & Springs
  - Build an Atom
  - Periodic Table
- Coding
- Drawing
- Lessons
- Games

Each app should expose a simple `create()` entry point and keep app logic outside `main.cpp`.

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

## Future OS launcher rule

For now, `main.cpp` can temporarily call one app directly.

Later, BitSlate OS should expose a launcher menu:

- STEM Apps
  - Circuit Lab
  - Collision Lab
  - Masses & Springs
- Coding
- Drawing
- Lessons
- Games

Each app should expose a simple `create()` entry point and keep app logic outside `main.cpp`.

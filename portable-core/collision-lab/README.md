# Collision Lab Portable Core

This folder contains pure Collision Lab logic for the future BitSlate embedded app.

It mirrors the Python desktop prototype in `desktop-prototypes/collision-lab-python/` and intentionally has no graphics, no LVGL, no LovyanGFX, and no Arduino dependency.

## Current Features

- Two balls
- 1D horizontal motion
- Wall bounce
- Elastic ball collision
- Momentum helper
- Kinetic energy helper
- Collision/wall-bounce flags for simple UI feedback

## Build Test

```powershell
g++ -std=c++17 portable-core/collision-lab/CollisionModel.cpp portable-core/collision-lab/test_collision_model.cpp -o portable-core/collision-lab/test_collision_model.exe
portable-core/collision-lab/test_collision_model.exe
```

## Next Step

Integrate this model into a BitSlate LVGL Collision Lab app after the core is verified. Keep rendering and touch handling outside this folder.

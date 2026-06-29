# Collision Lab Prototype Notes

## Desktop-First Workflow

Collision Lab has physics, controls, and animation. The desktop version lets us debug the model without also debugging LVGL, ESP32 timing, touch input, and display refresh.

## What We Learned From PhET

PhET separates the top-level model, ball system, play area, collision engine, and view controls. Balls have position, velocity, mass, radius, momentum, kinetic energy, and user-control state. The full PhET engine detects collisions before they happen and resolves the earliest collision in a time step.

For BitSlate v0, we are borrowing the concepts but not the architecture.

## Simplified 1D Elastic Logic

The first model uses two balls on a horizontal line. Each frame updates position from velocity, checks wall bounces, then checks whether the two balls overlap while moving toward each other. If so, it applies the 1D elastic collision equations and separates the balls slightly to avoid sticking.

## Intentionally Omitted

- 2D motion
- Angle controls
- Dragging balls
- Draggable velocity vectors
- Inelastic collisions
- Variable ball count
- Momentum diagram
- Graphs
- Embedded LVGL integration

## Next Steps

- Refine the desktop model if the feel is off.
- Keep the Python prototype as the visual sandbox.
- Use the matching C++ portable core in `portable-core/collision-lab/` as the future embedded logic source.
- Add a simple embedded LVGL Collision Lab after the core is stable.

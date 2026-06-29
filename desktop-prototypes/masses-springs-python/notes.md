# Masses & Springs Prototype Notes

## PhET Concepts Decoded

PhET's Masses & Springs model separates global simulation state, masses, springs, gravity, damping, and UI/tool state. A spring tracks natural length, displacement, spring constant, damping, attached mass, equilibrium position, and derived forces/energies. A mass tracks position, velocity, mass value, spring attachment, gravity, and derived net force/acceleration.

## Why BitSlate Simplifies

The PhET implementation is built for a full-featured browser simulation with multiple screens, a property system, accessibility instrumentation, energy views, vector displays, and exact damped oscillator math. BitSlate v0 needs a small, understandable classroom interaction that fits 480x320 and can later run on an ESP32-S3.

## Equations Used

- Spring force: `F_spring = -k * displacement`
- Gravity force: `F_gravity = m * g`
- Damping force: `F_damping = -b * velocity`
- Net force: `F_net = F_spring + F_gravity + F_damping`
- Acceleration: `a = F_net / m`
- Semi-implicit Euler integration:
  - `velocity += acceleration * dt`
  - `displacement += velocity * dt`
- Equilibrium extension: `x = mg / k`

## Intentionally Omitted

- multiple springs
- energy graph
- period timer
- precise PhET property system
- full view architecture
- named planets beyond a simple gravity slider
- vector overlays

## Next Step

Create a portable C++ core that mirrors the Python model, then integrate that core into a future embedded LVGL Masses & Springs app.

## Portable C++ Core

- C++ portable core now mirrors the Python model.
- Python remains the visual sandbox for interaction and feel.
- C++ core is the future embedded logic source.

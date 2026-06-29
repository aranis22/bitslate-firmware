# Portable Masses & Springs Core

Pure Masses & Springs / Hooke's Law logic for a future BitSlate embedded app.

This mirrors the Python desktop prototype in `desktop-prototypes/masses-springs-python/`.

It has no graphics, no LVGL dependency, no LovyanGFX dependency, and no Arduino dependency.

## Current Features

- one spring
- one mass
- Hooke's law
- gravity
- damping
- drag/release state
- equilibrium extension helper
- spring, gravity, damping, and net force helpers
- semi-implicit Euler stepping
- defensive clamps for child-friendly sliders and embedded stability

## Compile Test

```bash
g++ -std=c++17 portable-core/masses-springs/SpringMassModel.cpp portable-core/masses-springs/test_spring_mass_model.cpp -o portable-core/masses-springs/test_spring_mass_model.exe
portable-core/masses-springs/test_spring_mass_model.exe
```

## Next Step

After this core is verified, integrate it into a BitSlate LVGL Masses & Springs app.

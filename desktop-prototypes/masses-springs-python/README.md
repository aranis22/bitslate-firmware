# BitSlate Masses & Springs Prototype

Desktop Masses & Springs / Hooke's Law prototype for BitSlate.

This is not embedded firmware. It is a pygame prototype for validating interaction and physics before creating a portable C++ core and then a BitSlate LVGL app.

## Run

```bash
python main.py
```

## Test

```bash
python test_spring_model.py
```

## Current Features

- one spring
- one mass
- Hooke's law
- gravity
- damping
- drag and release
- sliders for mass, spring constant, damping, and gravity
- displacement and force readouts

## Future Path

Python model -> portable C++ core -> BitSlate LVGL app.

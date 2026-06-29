# BitSlate Desktop Collision Lab Prototype

This is a desktop Python prototype for BitSlate's Collision Lab.

It is not embedded firmware. The goal is to build and test the simulation logic on desktop before extracting a portable C++ core and later integrating into the BitSlate LVGL app.

## Run

```powershell
python -m pip install -r desktop-prototypes/collision-lab-python/requirements.txt
python desktop-prototypes/collision-lab-python/main.py
```

## Test

```powershell
python desktop-prototypes/collision-lab-python/test_collision_model.py
```

## Current Features

- 480x320 BitSlate-shaped window.
- Two 1D balls moving on a horizontal track.
- Play/pause.
- Reset.
- Mass sliders for Ball A and Ball B.
- Velocity sliders for Ball A and Ball B.
- Elastic collision between balls.
- Wall bounce.
- Velocity arrows.
- Total momentum and kinetic energy text.

## Future Path

Python model -> portable C++ core -> BitSlate LVGL app.

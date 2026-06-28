# BitSlate Desktop Circuit Lab Prototype

This is the first desktop Python prototype for BitSlate's Circuit Lab.

It is not embedded firmware. The goal is to debug the simulation and interaction logic on desktop first, then port the stable core model to C++, then integrate it into the BitSlate LVGL app.

## Run

```powershell
python -m pip install -r desktop-prototypes/circuit-lab-python/requirements.txt
python desktop-prototypes/circuit-lab-python/main.py
```

## Current Features

- 480x320 window matching the BitSlate landscape aspect ratio.
- Left tool tray for Battery, Bulb, Wire, and Clear.
- Place one or more batteries and bulbs on the canvas.
- Draw wires between nearby terminals with snapping.
- Drag placed components.
- Detect a simple closed circuit path.
- Show bulb OFF/ON state visually.

## Future Port Path

Python logic -> C++ model -> BitSlate LVGL app.

The embedded display, touch, LovyanGFX, and LVGL stack should stay separate until this model is small, tested, and fun to use.

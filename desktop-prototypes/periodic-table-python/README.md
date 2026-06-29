# Periodic Table Desktop Prototype

This folder is for the BitSlate desktop prototype of the Periodic Table STEM app.

This is not embedded firmware. The intended workflow is:

1. Python desktop prototype.
2. Portable C++ core.
3. BitSlate LVGL app.

The app is a compact 480x320 interactive periodic table inspired by ptable.com Properties mode. It uses a left info panel and a compressed periodic table grid. Click an element cell to update the selected element details.

## Current v0

- 480x320 pygame window.
- Left selected-element info panel.
- Right compressed periodic table grid.
- All 118 elements included with correct symbol, name, atomic number, and grid position.
- Category colors.
- Selected element highlight.
- Default selected element: Carbon.
- No temperature slider, animations, embedded LVGL, or large property list.

## Planned Run Command

```powershell
python desktop-prototypes/periodic-table-python/main.py
```

## Planned Test Command

```powershell
python desktop-prototypes/periodic-table-python/test_periodic_model.py
```

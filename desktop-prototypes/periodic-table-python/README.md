# Periodic Table Desktop Prototype

This folder is for the BitSlate desktop prototype of the Periodic Table STEM app.

This is not embedded firmware. The intended workflow is:

1. Python desktop prototype.
2. Portable C++ core.
3. BitSlate LVGL app.

The app target is a compact 480x320 interactive periodic table inspired by ptable.com Properties mode. The first prototype should focus on selecting an element and updating a lean info panel.

## Planned Run Command

```powershell
python desktop-prototypes/periodic-table-python/main.py
```

## Planned Test Command

```powershell
python desktop-prototypes/periodic-table-python/test_periodic_model.py
```


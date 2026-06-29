# Build an Atom Desktop Prototype

This folder is for the BitSlate desktop prototype of Build an Atom.

It is not embedded firmware. The intended workflow is:

1. Python model and pygame prototype.
2. Portable C++ core.
3. BitSlate LVGL app.

The first prototype should stay small: proton, neutron, and electron counts; a simple nucleus and shell display; and readouts for element, symbol, atomic number, mass number, and charge.

## Run

```powershell
python desktop-prototypes/build-atom-python/main.py
```

## Test

```powershell
python desktop-prototypes/build-atom-python/test_atom_model.py
```


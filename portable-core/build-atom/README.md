# Build an Atom Portable Core

This folder contains the portable C++ core for the BitSlate Build an Atom app.

The core is intentionally independent of Arduino, LVGL, LovyanGFX, pygame, and embedded display code. It is small enough to be reused by the desktop prototype and later by the embedded LVGL app.

## Model

- Tracks protons, neutrons, and electrons.
- Atomic number is proton count.
- Mass number is protons plus neutrons.
- Charge is protons minus electrons.
- Shell 1 capacity is 2 electrons.
- Shell 2 capacity is 8 electrons.
- Supports the first 10 elements:
  - 0 unknown
  - 1 H
  - 2 He
  - 3 Li
  - 4 Be
  - 5 B
  - 6 C
  - 7 N
  - 8 O
  - 9 F
  - 10 Ne

## Desktop Test

```powershell
g++ -std=c++17 portable-core/build-atom/AtomModel.cpp portable-core/build-atom/test_atom_model.cpp -o portable-core/build-atom/test_atom_model.exe
portable-core/build-atom/test_atom_model.exe
```


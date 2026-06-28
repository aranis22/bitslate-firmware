# Circuit Lab Portable Core

This folder contains pure Circuit Lab logic for the future BitSlate embedded app.

It has no graphics, no LVGL, no LovyanGFX, and no Arduino dependencies. It mirrors the Python desktop prototype model so the logic can be tested before it is integrated into the BitSlate firmware UI.

## Current Feature

- Battery components
- Bulb components
- Wires between terminals
- Simplified closed-loop graph detection
- Bulb ON/OFF result

The battery terminals are not internally connected. Bulb terminals are internally connected. A bulb lights when a battery positive terminal can reach the battery negative terminal through wires and a path that includes a bulb.

## Build Test

```powershell
g++ -std=c++17 portable-core/circuit-lab/CircuitModel.cpp portable-core/circuit-lab/test_circuit_model.cpp -o portable-core/circuit-lab/test_circuit_model.exe
portable-core/circuit-lab/test_circuit_model.exe
```

## Next Embedded Step

Integrate this model into the BitSlate LVGL Circuit Lab UI after the portable logic remains stable. Do not add display, touch, or LVGL code to this folder.

# Circuit Lab Prototype Notes

## Why Desktop First

Circuit Lab has interaction, graphics, and simulation logic. Building it first on desktop lets us debug the learning model without also debugging ESP32 timing, touch mapping, LVGL, and display flush behavior.

## Simplified Graph Logic

The first model treats a circuit as terminals connected by wires. A bulb has an internal edge between its two terminals. A battery does not internally connect its positive and negative terminals.

The bulb turns on when there is a path from a battery positive terminal to a battery negative terminal that passes through that bulb's internal connection.

This is intentionally educational v0 logic, not a full electrical solver.

## Portable Model Shape

The Python model now uses the same core concepts as the C++ portable version:

- `ComponentType`
- `Terminal`
- `Component`
- `Wire`
- `CircuitModel`

The C++ core lives in `portable-core/circuit-lab/`. Embedded integration comes later after the core stays stable.

## Intentionally Not Implemented Yet

- Resistors
- Switches
- Current animation
- Voltage/current math
- MNA solver
- Multiple lesson modes
- Save/load
- Teacher mode

## Later Extensions

- Switches that open/close graph paths
- Resistors with simple brightness effects
- Current animation along connected wires
- Bulb brightness based on simple resistance totals
- Lesson prompts and guided challenges
- C++ port of the pure model

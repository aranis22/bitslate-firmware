# PhET Research Summary for BitSlate

## Why we are doing this

Complex STEM apps should not be debugged across physics logic, graphics, touch, LVGL, LovyanGFX, and ESP32 constraints all at once. The safer BitSlate workflow is:

- Prototype complex STEM apps on desktop first.
- Extract and test the pure simulation logic.
- Port stable logic to C++.
- Integrate the proven model into BitSlate/LVGL.

BitSlate remains the final target. The desktop prototype is the learning and debugging bench.

## Repos inspected

- `collision-lab`: cloned successfully.
- `circuit-construction-kit-dc`: cloned successfully.
- `circuit-construction-kit-common`: cloned successfully.
- `forces-and-motion-basics`: cloned successfully.
- `masses-and-springs`: cloned successfully.
- `wave-interference`: cloned successfully.

## Collision Lab

Core model files are under `collision-lab/js/common/model/`, especially `CollisionLabModel.js`, `Ball.js`, `BallSystem.js`, `CollisionEngine.js`, `Collision.js`, and screen-specific model/engine classes.

The main structure is model state plus a step loop. `CollisionLabModel` tracks play state and elapsed time, owns a `BallSystem`, and delegates motion/collision updates to a collision engine. `Ball` stores position, velocity, mass, radius, and derived momentum/speed. The collision engine computes ball-ball and wall collisions, including collision timing and velocity updates.

For BitSlate, reuse the concepts of simple bodies with position, velocity, mass, and step-based updates. Simplify away PhET's full property system, instrumentation, screen variants, and high-precision collision scheduling at first.

## Circuit Construction Kit DC

The DC repo is mostly the app/screen layer. The important model entry points include:

- `circuit-construction-kit-dc/js/intro/model/IntroModel.ts`
- `circuit-construction-kit-dc/js/lab/model/LabModel.ts`
- `circuit-construction-kit-dc/dependencies.json`

Both DC screen models extend the shared model in `circuit-construction-kit-common`. The DC repo selects modes/options; the reusable circuit logic lives mostly in common.

Useful concepts: components are placed on a board, wires connect endpoints, terminals become graph nodes, and circuit state is stepped after edits. For BitSlate, keep the visible tray/canvas idea, but start with a much smaller model.

## Circuit Construction Kit Common

Main shared logic lives under `circuit-construction-kit-common/js/model/`.

Important classes and concepts:

- `CircuitConstructionKitModel.ts`: top-level model, owns a `Circuit`, tool/meter state, display flags, zoom, stopwatch, and update loop.
- `Circuit.ts`: owns circuit elements, charges, selected item, dirty state, groups, and change emitters.
- `CircuitElement.ts`: base for two-terminal elements with start/end vertices, current, voltage difference, and path length.
- `Vertex.ts`: terminal/node object with position, voltage, draggable/attachable state, and labels.
- `Wire.ts`, `Battery.ts`, `Resistor.ts`, `Switch.ts`, `LightBulb.ts`: component models.
- `analysis/mna/MNACircuit.ts`: modified nodal analysis solver for voltages/currents.

The model represents components as edges between vertices. Wires are low-resistance connectors, batteries are voltage sources, resistors/light bulbs/switches are circuit elements, and solving uses graph/connectivity plus MNA for real current/voltage behavior.

For BitSlate, use the representation concept, not the implementation. Start with terminals, components, wires, and a graph closed-loop check. Delay MNA, charge animation, meters, accessibility descriptions, and full edit tools.

## Forces and Motion Basics

Useful model files include `forces-and-motion-basics/js/motion/model/MotionModel.ts` and `forces-and-motion-basics/js/netforce/model/NetForceModel.ts`.

The motion model tracks applied force, friction, net force, mass, position, speed, velocity, acceleration, and play state. It computes friction from mass, gravity, coefficient, motion state, and applied force. The step loop updates net force, acceleration, velocity, and position. The net-force screen is a simpler tug-of-war model with opposing forces and cart motion.

For BitSlate, reuse the simple state/update-loop idea: force inputs, mass, acceleration, velocity, position. Avoid PhET's full control/property/view architecture.

## Masses and Springs

Core files are under `masses-and-springs/js/common/model/`, especially `MassesAndSpringsModel.js`, `Mass.js`, `Spring.js`, and screen models such as `LabModel.js` and `EnergyModel.js`.

The model owns arrays of masses and springs, gravity, damping, visibility flags, and attach/detach behavior. `Spring` tracks spring constant, natural resting length, damping, displacement, attached mass, equilibrium position, and step-based oscillation. Useful equations include equilibrium extension `mg/k` and damped mass-spring motion.

For BitSlate, begin with one spring and one mass: drag a mass, attach it to a spring, then animate a simplified Hooke's-law oscillator. Delay energy graphs, multi-screen modes, period traces, and precise analytic damping cases.

## Wave Interference

Important files include:

- `wave-interference/js/waves/model/WavesModel.ts`
- `wave-interference/js/common/model/Scene.ts`
- `wave-interference/js/common/model/WaterScene.ts`
- `wave-interference/js/common/model/SoundScene.ts`
- `wave-interference/js/slits/model/SlitsModel.ts`
- `wave-interference/js/interference/model/InterferenceModel.ts`

The wave model uses scenes for water/sound/light, a lattice/grid for wave values, oscillator source state, frequency, amplitude, phase, wave speed, and a stepped time model. Scenes can represent point or plane waves, pulse or continuous disturbance, and barriers/slits.

For BitSlate, waves are likely too complex for the first STEM app. A future version should start with a small grid, one source, amplitude/frequency sliders, and a simple ripple update before adding slits/interference.

## Circuit Lab: simplified BitSlate plan

Build a mini desktop Circuit Lab first:

- Use Python first.
- Use a 2D canvas.
- Add draggable components.
- Give each component visible nodes/terminals.
- Let wires connect terminals.
- Implement a simple graph/path check first.
- Detect a battery + bulb closed loop.
- Later add resistor, switch, current animation, and brightness.
- Port the core model to C++ only after the desktop logic feels solid.

The first embedded return should integrate a proven C++ model into the existing LVGL app shell, not invent the model directly on-device.

## Recommendation

Build the mini desktop Circuit Lab first. It directly supports the current BitSlate Circuit Lab direction and can start with graph connectivity instead of full electronics physics. That gives quick educational value: students can place a battery, bulb, and wires, then see whether the circuit is complete. More accurate current, resistance, switches, and animation can layer in after the interaction model is stable.

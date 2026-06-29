# Masses & Springs Decode Notes

## Files inspected

- `research/phet-reference/masses-and-springs/README.md`
- `research/phet-reference/masses-and-springs/dependencies.json`
- `research/phet-reference/masses-and-springs/js/common/MassesAndSpringsConstants.js`
- `research/phet-reference/masses-and-springs/js/common/model/MassesAndSpringsModel.js`
- `research/phet-reference/masses-and-springs/js/common/model/Spring.js`
- `research/phet-reference/masses-and-springs/js/common/model/Mass.js`
- `research/phet-reference/masses-and-springs/js/common/model/Body.js`
- `research/phet-reference/masses-and-springs/js/lab/model/LabModel.js`
- `research/phet-reference/masses-and-springs/js/energy/model/EnergyModel.js`
- `research/phet-reference/masses-and-springs/js/common/view/GravityAndDampingControlNode.js`
- `research/phet-reference/masses-and-springs/js/common/view/SpringControlPanel.js`
- `research/phet-reference/masses-and-springs/js/common/view/OneSpringScreenView.js`
- `research/phet-reference/masses-and-springs/js/common/view/MassNode.js`
- `research/phet-reference/masses-and-springs/js/lab/model/PeriodTrace.js`

## Core model concepts

PhET separates the simulation into a shared `MassesAndSpringsModel`, individual `Mass` objects, and individual `Spring` objects. The base model owns global state such as play/pause, gravity, damping, visible tools, masses, springs, and the model step loop.

The spring object stores the spring anchor position, natural/resting length, spring constant `k`, displacement from natural length, damping coefficient, attached mass pointer, equilibrium position, and several derived energy values. Spring force is derived from Hooke's law as the negative spring constant times displacement.

The mass object stores mass value, position, center of mass, vertical velocity, vertical acceleration, whether it is user controlled, whether it is attached to a spring, and derived forces/energies. Net force is derived from spring force minus weight, and acceleration is net force divided by mass.

Position and displacement use a vertical model coordinate system. The spring bottom position is derived from top position plus spring length, where spring length depends on natural resting length and displacement. An attached mass follows the spring bottom plus hook offset.

Gravity is a model property shared by masses and springs. PhET includes named bodies such as Earth, Moon, Jupiter, and custom gravity. Damping is a system property linked into each spring as a damping coefficient.

Natural/rest length is the unloaded spring length. Equilibrium extension is computed from `x = mg/k`, then used to place the equilibrium line and the center-of-mass equilibrium point for an attached mass.

Attach/detach behavior is interaction driven. A dragged mass attaches when it is close enough to the spring bottom and horizontally aligned with the spring. A mass detaches when dragged laterally beyond a release threshold. BitSlate v0 can simplify this to one always-attached mass, or one tap/drag attach target later.

The step/update loop clamps or scales `dt`, advances unattached masses under gravity, advances attached springs, and updates timers/traces. PhET uses an analytic damped oscillator solution for attached spring motion rather than a simple Euler integrator.

## Useful equations/concepts

- Hooke's law: `F_spring = -k x`
- Gravity force: `F_gravity = m g`
- Damping force for BitSlate v0: `F_damping = -b v`
- Net force: `F_net = F_spring + F_gravity + F_damping`, with signs chosen for the BitSlate coordinate convention
- Acceleration: `a = F_net / m`
- Velocity integration: `v += a * dt`
- Position integration: `x += v * dt`
- Equilibrium extension: `x_eq = mg / k`
- Elastic potential energy: `0.5 * k * x^2`
- Kinetic energy: `0.5 * m * v^2`
- Gravitational potential energy: `m g h`

For BitSlate v0, a small semi-implicit Euler integrator should be enough:

1. Measure displacement from the spring's natural length.
2. Compute spring, gravity, and damping forces.
3. Compute acceleration.
4. Update velocity.
5. Update position.
6. Clamp extreme values so the embedded UI stays stable.

## UI concepts from PhET

- Mass selection uses visible mass objects with different values and mystery labels.
- Spring constant can be adjusted with a slider/control panel.
- Damping can be adjusted with a slider on screens that expose damping.
- Gravity can be selected from named bodies or changed with a slider.
- Ruler, natural length, movable line, and equilibrium markers can be toggled.
- Play/pause, step, reset, stopwatch, and slow motion are part of the richer simulation shell.
- Period tracing detects crossings/peaks around equilibrium.
- Energy mode shows kinetic, gravitational, elastic, thermal, and total energy.
- Vector modes can show velocity, acceleration, gravity force, spring force, or net force.

## What BitSlate should simplify

- one spring
- one mass
- vertical motion only
- spring constant slider
- mass slider
- damping slider
- gravity toggle or slider
- play/pause/reset
- no graphs at first
- no multiple springs at first
- no energy mode at first
- no advanced measurement tools at first

## Proposed BitSlate v0 desktop prototype

- Python pygame first
- 480x320 window
- spring hanging from top
- mass block/circle attached
- sliders for mass, `k`, damping
- gravity on/off or gravity value
- drag mass down and release
- animate oscillation
- show displacement and force values
- later port pure model to C++
- then embedded LVGL app

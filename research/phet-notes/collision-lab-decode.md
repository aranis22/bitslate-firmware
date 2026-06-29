# Collision Lab Decode Notes

## PhET files inspected

- `research/phet-reference/collision-lab/README.md`
- `research/phet-reference/collision-lab/dependencies.json`
- `research/phet-reference/collision-lab/js/common/model/CollisionLabModel.js`
- `research/phet-reference/collision-lab/js/common/model/Ball.js`
- `research/phet-reference/collision-lab/js/common/model/BallSystem.js`
- `research/phet-reference/collision-lab/js/common/model/BallState.js`
- `research/phet-reference/collision-lab/js/common/model/BallUtils.js`
- `research/phet-reference/collision-lab/js/common/model/CollisionEngine.js`
- `research/phet-reference/collision-lab/js/common/model/Collision.js`
- `research/phet-reference/collision-lab/js/common/model/PlayArea.js`
- `research/phet-reference/collision-lab/js/common/model/MomentaDiagram.js`
- `research/phet-reference/collision-lab/js/explore1D/model/Explore1DBallSystem.js`
- `research/phet-reference/collision-lab/js/explore1D/model/Explore1DCollisionEngine.js`
- `research/phet-reference/collision-lab/js/common/CollisionLabConstants.js`
- `research/phet-reference/collision-lab/js/common/view/CollisionLabScreenView.js`
- `research/phet-reference/collision-lab/js/common/view/CollisionLabControlPanel.js`
- `research/phet-reference/collision-lab/js/common/view/CollisionLabTimeControlNode.js`
- `research/phet-reference/collision-lab/js/common/view/BallValuesPanel.js`
- `research/phet-reference/collision-lab/js/common/view/BallMassSlider.js`
- `research/phet-reference/collision-lab/js/common/view/BallVelocityVectorNode.js`
- `research/phet-reference/collision-lab/js/common/view/BallMomentumVectorNode.js`

## Core model concepts

PhET's top-level `CollisionLabModel` owns play/pause state, elapsed time, time speed, a `PlayArea`, a `BallSystem`, a `CollisionEngine`, and a momentum diagram. The step loop only advances when playing. A manual step increments elapsed time and delegates physics to the collision engine.

A `Ball` stores center position, velocity, mass, derived speed, derived momentum, radius, path history, and user-control flags. Radius can be derived from mass or held constant. In 1D screens, y position and y velocity are constrained to zero.

`BallSystem` keeps persistent ball instances, controls how many are active, resets/restarts ball state, updates paths, and computes total kinetic energy from active balls. It also saves restart states after user manipulation.

`PlayArea` stores bounds, dimension, reflecting-border state, grid visibility, and elasticity. For v0, the useful pieces are wall bounds and elasticity. PhET has an elasticity range from fully inelastic to fully elastic; our first version will use elastic collisions only.

`CollisionEngine` detects ball-ball and ball-border collisions before they happen to avoid tunneling. It creates `Collision` records with a possible collision time, advances to the earliest collision, resolves it, invalidates affected collision records, and repeats until the step is consumed. Ball-ball response uses normal/tangent components, masses, and elasticity. Border response flips the relevant velocity component.

Momentum is mass times velocity. Kinetic energy is `0.5 * mass * velocity^2`. PhET also has a momenta diagram and optional momentum vectors.

## UI concepts

Useful PhET UI ideas:

- A play area with clear wall/boundary lines.
- Play/pause, step, restart, and reset controls.
- Ball value panel with mass, position, velocity, and momentum.
- Mass sliders per ball.
- Velocity vectors drawn from balls, with draggable vector tips in the full sim.
- Momentum vectors and kinetic energy visibility toggles.
- A total momentum diagram.
- A control panel for velocity/momentum visibility, kinetic energy, values, reflecting border, path visibility, elasticity, and constant-size balls.

For BitSlate v0, the useful subset is a visible track, two balls, velocity arrows, mass sliders, velocity sliders, play/pause, reset, and text readouts for total momentum and kinetic energy.

## What we will simplify

- No full PhET property system.
- No scene graph.
- No multi-screen architecture.
- No accessibility/instrumentation layer.
- No high-precision collision scheduling initially.
- No full 2D vector UI at first.
- No draggable velocity-vector tips.
- No momentum diagram.
- No elasticity slider yet.
- No variable ball count.
- No saved/restart state distinction beyond reset.

## Desktop prototype plan

Collision Lab v0:

- 480x320 pygame window.
- Two balls.
- 1D horizontal motion first.
- Mass sliders from 1 to 5.
- Velocity sliders from -120 to 120 px/sec.
- Play/pause.
- Reset.
- Elastic collision.
- Optional wall bounce enabled.
- Display total momentum and kinetic energy as text.
- Draw simple velocity arrows near the balls.

The desktop model should remain independent from pygame so the logic can later become a portable C++ core and then an embedded LVGL app.

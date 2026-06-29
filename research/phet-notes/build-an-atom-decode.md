# Build an Atom Decode Notes

## Files inspected

- `research/phet-reference/build-an-atom/README.md`
- `research/phet-reference/build-an-atom/dependencies.json`
- `research/phet-reference/build-an-atom/doc/model.md`
- `research/phet-reference/build-an-atom/doc/implementation-notes.md`
- `research/phet-reference/build-an-atom/js/common/model/BAAModel.ts`
- `research/phet-reference/build-an-atom/js/common/model/BAAParticle.ts`
- `research/phet-reference/build-an-atom/js/common/view/InteractiveSchematicAtom.ts`
- `research/phet-reference/build-an-atom/js/common/view/BAAParticleView.ts`
- `research/phet-reference/build-an-atom/js/common/view/ChargeMeter.ts`
- `research/phet-reference/build-an-atom/js/atom/AtomScreen.ts`
- `research/phet-reference/build-an-atom/js/atom/view/AtomScreenView.ts`
- `research/phet-reference/build-an-atom/js/symbol/SymbolScreen.ts`
- `research/phet-reference/build-an-atom/js/symbol/view/SymbolScreenView.ts`
- `research/phet-reference/build-an-atom/js/symbol/view/BAASymbolNode.ts`
- `research/phet-reference/build-an-atom/js/game/model/GameModel.ts`
- `research/phet-reference/build-an-atom/assets/`
- `research/phet-reference/build-an-atom/images/`

## Core model concepts

- Users build atoms from protons, neutrons, and electrons.
- Protons and neutrons live in the nucleus.
- Electrons live in electron shells/orbits around the nucleus.
- Element identity is determined by proton count, also known as atomic number.
- Mass number is proton count plus neutron count.
- Net charge is proton count minus electron count.
- Ion/neutral state follows from net charge: zero is neutral, positive is cation-like, negative is anion-like.
- Isotope identity follows from element plus neutron count or mass number.
- PhET allows intentionally unrealistic atoms so the learning focus stays on counting particles and reading atom properties.
- PhET can show stable/unstable nucleus state, but it does not depict nuclear decay.
- Electron shell behavior in the reference is simplified: shell 1 holds 2 electrons and shell 2 holds 8 electrons for the first two periodic-table rows.

## Likely PhET architecture

- `BAAModel` is the main interactive model for atom-building screens.
- `BAAParticle` wraps a shared `Particle` type with a particle kind: proton, neutron, or electron.
- `ParticleAtom` stores particle references and derives atom properties.
- `NumberAtom` stores particle counts only and is useful for challenges, answers, and setting atom state.
- The Atom and Symbol screens share most model/view code.
- The Symbol screen adds a chemical-symbol display.
- Particle buckets provide draggable protons, neutrons, and electrons.
- Drag/release logic decides whether a particle is captured by the nucleus, captured by the electron shell, or returned to its bucket.
- Game mode is state-driven and uses challenge model/view classes for prompts like count-to-symbol, schematic-to-charge, and symbol-to-counts.
- The reference keeps model state separate from view classes, which fits the BitSlate workflow: Python prototype, portable C++ core, embedded LVGL UI.

## UI concepts useful for BitSlate

- Draggable or tap-place proton, neutron, and electron trays.
- Nucleus drop zone for protons and neutrons.
- Electron shell drop zone for electrons.
- Visual shells around a central nucleus.
- Element name and symbol display.
- Atomic number, mass number, and net charge readouts.
- Neutral/ion indicator.
- Stable/unstable indicator as a later optional feature.
- Simple challenge prompts:
  - Build Helium.
  - Make a neutral Lithium atom.
  - Build an atom with mass number 4.
  - Make an atom with charge +1.

## What BitSlate should simplify

- No full periodic table initially.
- No advanced isotope stability model initially.
- No PhET game mode initially.
- No complex electron orbital rules at first.
- Start with the first 10 elements, or even the first 3 to 5 for the first visual pass.
- Use a simple shell capacity model:
  - Shell 1 holds 2 electrons.
  - Shell 2 holds 8 electrons.
- Element identity should be based on proton count.
- Mass number should be `protons + neutrons`.
- Charge should be `protons - electrons`.

## Proposed BitSlate v0 desktop prototype

- Python pygame first.
- 480x320 window.
- Left tray:
  - Proton
  - Neutron
  - Electron
- Center atom workspace:
  - Nucleus circle.
  - Electron shells.
- Drag or click particles into the atom.
- Show:
  - Element name.
  - Symbol.
  - Atomic number.
  - Mass number.
  - Charge.
- Add a reset button.
- Add simple challenge prompts later:
  - Build Helium.
  - Make a neutral Lithium atom.


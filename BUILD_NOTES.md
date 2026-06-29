# Build Notes

## 2026-06-27 Checkpoint

- LovyanGFX display test works on the ESP32-S3 with the confirmed ILI9488 pinout.
- Touch works with the XPT2046-style resistive touch controller on shared SPI.
- Touch X mirroring is corrected inside `LGFX_BitSlate::getTouch(...)`.
- LVGL is not yet re-enabled.
- Next step: re-enable LVGL carefully after this GitHub checkpoint.

## 2026-06-27 LVGL Touch Checkpoint

- LVGL touch confirmation works on hardware.
- Button changes from `Touch me` to `Touched!`.
- Status label changes to green `Touch OK`.
- This confirms the LovyanGFX-to-LVGL touch bridge works.
- LVGL display flush works.
- Next step: build the real BitSlate home screen / launcher UI.
- Do not change verified display pins.

## 2026-06-27 Circuit Lab Early Prototype

- Simple Circuit Lab / STEM app screen exists.
- Wire placement is partially working.
- Wire placement behavior is weird but interesting.
- Current limitation: cannot drag wire directly from the left tray as expected.
- Frozen for now.
- Next direction is PhET/source research and desktop prototype workflow before continuing embedded simulation logic.

## 2026-06-28 PhET Research Workflow

- PhET reference repositories were cloned into ignored `research/phet-reference/`.
- BitSlate research notes live in `research/phet-notes/`.
- Complex STEM simulation workflow is now desktop prototype first, then pure logic, then C++ port, then LVGL integration.
- Circuit Lab should continue from a small desktop model before returning to embedded UI details.

## 2026-06-28 Desktop Circuit Lab Prototype

- Started a Python/pygame desktop prototype for BitSlate Circuit Lab.
- Prototype is intentionally separate from embedded LVGL firmware.
- Initial model uses components, terminals, wires, and simplified graph/path closed-loop detection.
- First goal is battery + bulb + wire logic before any C++ or LVGL integration.

## 2026-06-28 Circuit Lab Portable Core

- Desktop prototype worked visually with battery, bulb, wires, closed-loop detection, and bulb ON state.
- Python model was refactored into a portable shape using component types, terminals, wires, and graph/path logic.
- C++ portable core was created in `portable-core/circuit-lab/`.
- C++ core mirrors the Python graph/path logic.
- Embedded LVGL integration is next, but not done in this checkpoint.

## 2026-06-29 STEM Apps Roadmap

- Embedded Circuit Lab prototype was checkpointed on `circuit-lab-embedded-core`.
- Circuit Lab is now tracked as the first modular STEM app.
- Collision Lab v0 is the next planned STEM app.
- Workflow remains desktop prototype, portable model/core, embedded LVGL app, then polish.

## 2026-06-29 Desktop Collision Lab Prototype

- Started Collision Lab desktop prototype on `desktop-collision-lab-prototype`.
- Decoded PhET Collision Lab model and UI references.
- Built simplified 1D Python prototype with two balls, mass/velocity sliders, play/pause/reset, elastic collision, wall bounce, momentum, and kinetic energy.
- Added plain Python model tests.
- Embedded Collision Lab port is not started yet.

## 2026-06-29 Collision Lab Portable Core

- Python Collision Lab prototype worked visually.
- C++ portable core was created in `portable-core/collision-lab/`.
- C++ core mirrors the 1D elastic collision model from the Python prototype.
- Embedded LVGL integration is not started yet.

## 2026-06-29 STEM v0 Checkpoints and Masses & Springs Research

- Circuit Lab v0 and Collision Lab v0 are checkpointed.
- Apps are being documented as future BitSlate OS modules.
- Masses & Springs / Hooke's Law research started.
- Forces & Motion and Wave Interference are skipped for now.
- Do not change the verified display pins while continuing STEM app work.

## 2026-06-29 Desktop Masses & Springs Prototype

- Started Masses & Springs desktop prototype.
- Uses one spring and one mass.
- Implements Hooke's law, gravity, damping, and drag/release.
- Embedded LVGL integration is not started.

## 2026-06-29 Masses & Springs Portable Core

- Desktop Masses & Springs prototype worked visually.
- C++ portable core created in `portable-core/masses-springs/`.
- C++ core mirrors Hooke's law, gravity, damping, and drag/release model.
- Embedded LVGL integration is not started yet.

## 2026-06-29 Embedded Masses & Springs v0

- Embedded Masses & Springs v0 works visually.
- UI fixes include equilibrium label placement, right control panel spacing, and raised bottom-left readouts.
- Spring/mass animation moves while Play is pressed.
- Build/upload worked on ESP32-S3.
- Next STEM app research target is Build an Atom.

## 2026-06-29 Build an Atom Research Start

- Masses & Springs embedded v0 was checkpointed.
- Build an Atom research started on `build-atom-research`.
- PhET Build an Atom reference repo was cloned into ignored `research/phet-reference/` and inspected.
- Desktop prototype scaffold created in `desktop-prototypes/build-atom-python/`.
- Implementation is not started yet beyond a small model scaffold.

## 2026-06-29 Build an Atom Portable Core

- Build an Atom Python desktop prototype works visually.
- Portable C++ core created in `portable-core/build-atom/`.
- C++ core tracks protons, neutrons, electrons, atomic number, mass number, charge, neutrality, and first-10-element lookup.
- C++ core has assert tests for empty atom, H, He, Li ion, electron shell capacity, and reset.
- Embedded LVGL integration is not started yet.

## 2026-06-29 Build an Atom Desktop Prototype Update

- Desktop pygame prototype has clickable Proton, Neutron, Electron, and Reset tray buttons.
- Tray buttons show matching colored particle icons.
- Center atom area shows nucleus, shell 1, shell 2, particle counts, and readouts.
- Embedded LVGL integration is starting next on a separate branch.

## 2026-06-29 Embedded Build an Atom v0

- Build an Atom embedded v0 was created.
- UI refined with a right-side boxed element card.
- Atom diagram and readout overlap was fixed.
- Sidebar particle icon/text overlap was fixed.
- Build passed.
- Upload passed.
- Visual verification passed.
- No BitSlate OS launcher yet.
- No drag mode or game mode yet.

## 2026-06-29 Periodic Table Planning

- Periodic Table research/planning started.
- This is a BitSlate STEM app, not a PhET lab.
- Planned layout is a 480x320 screen with a left info panel and right compressed periodic table grid.
- Left panel will focus on atomic mass, category/series, group/period, electron shells, state at room temp, and electronegativity.
- Python prototype, portable C++ core, and embedded LVGL app are not implemented yet.

## 2026-06-29 Periodic Table Desktop Prototype

- Implemented the first pygame Periodic Table desktop prototype.
- Includes all 118 elements with symbol, name, atomic number, and periodic-grid position.
- Left info panel updates when an element is clicked.
- Right side shows a compressed, category-colored periodic table grid.
- Lanthanides and actinides are shown as bottom rows.
- Embedded LVGL integration is not started.

## 2026-06-29 Periodic Table Portable Core

- Periodic Table portable C++ core created in `portable-core/periodic-table/`.
- Core uses a static table of 118 elements.
- Supports lookup by atomic number and grid position.
- Default selected element is Carbon.
- Desktop Python prototype remains the reference UI for now.

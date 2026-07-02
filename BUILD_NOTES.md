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

## 2026-06-29 Embedded Periodic Table v0 and STEM Folder Grouping

- Embedded Periodic Table v0 is working on hardware.
- Periodic Table cells now show symbols only for readability.
- Left info panel text was tightened to fit the 480x320 layout.
- The periodic grid is clickable and updates the selected element.
- Category colors are working.
- No zoom/scroll yet.
- No temperature slider.
- No advanced property pages yet.
- Embedded STEM app folders are now grouped by subject:
  - Physics
    - Circuit Lab
    - Collision Lab
    - Masses & Springs
  - Chemistry
    - Build an Atom
    - Periodic Table
- Math
  - reserved for future apps
- PlatformIO still uses a temporary narrow build filter for the Periodic Table validation branch.

## 2026-06-29 Embedded Scientific Calculator v0

- Embedded Scientific Calculator v0 is working on hardware.
- Retro LVGL calculator UI uses the Monogram pixel font.
- Clickable keypad supports `+`, `-`, `x`, `/`, parentheses, `PI`, and `ANS`.
- Scientific functions work: `SQR`, `SIN`, `COS`, `TAN`, `LOG`, and `LN`.
- DEG/RAD toggle is integrated into the display area and updates trig evaluation mode.
- `%` and `+/-` are implemented for calculator input flow.
- Error handling shows `ERR` and `DIV 0`.
- No graphing, CAS, or equation solver yet.
- PlatformIO still uses a temporary narrow build filter for calculator validation on this branch.

## 2026-06-30 Embedded Graphing Calculator v0

- Embedded Graphing Calculator v0 is working on hardware.
- Retro LVGL graphing calculator UI uses the Monogram pixel font.
- Screen includes a graph panel, table panel, keypad, bottom display, and X/Y trace readouts.
- Trace X can be controlled by touch slider and rotary encoder on GPIO38/GPIO39.
- Trace updates refresh X/Y readouts, graph cursor, and generated table values.
- Parser supports `X`, constants, optional `Y=` prefix, implicit multiplication, parentheses, powers, and functions `SIN`, `COS`, `TAN`, `SQR`, `LOG`, and `LN`.
- Constant functions like `4` and `Y=4` graph as horizontal lines.
- No multi-function graphing yet.
- No zoom/pan yet.
- No equation list yet.
- PlatformIO still uses a temporary narrow build filter for graphing calculator validation on this branch.

## 2026-06-30 Chess Python Prototype Start

- Started BitSlate Chess in `src/apps/games/chess/`.
- This step is desktop Python prototype only.
- Prototype uses existing chess PNG assets from `src/assets/images/chess-piece-png/`.
- Board, pieces, turn indicator, selection, status, and reset UI were added.
- Model includes standard setup, turn alternation, legal moves for all normal pieces, captures, pawn double move, pawn diagonal capture, and auto-queen promotion.
- Check/checkmate, castling, en passant, AI, and network play are intentionally deferred.

## 2026-06-30 Embedded Chess v0

- Chess Python desktop prototype: working.
- LVGL chess asset conversion: working.
- LVGL embedded board render: working.
- Tap-to-move playable Chess v0 is working on hardware.
- Chess pieces use 24x24 LVGL ARGB8888 bitmaps generated from the PNG source assets.
- Board is centered at 304x304 with 38x38 clickable squares.
- Embedded chess model supports standard starting position, white/black turns, pawns, rooks, knights, bishops, queens, kings, captures, own-piece capture prevention, sliding-piece blocking, pawn double move, pawn diagonal capture, and auto-queen promotion.
- Check/checkmate, castling, en passant, stalemate, timers, AI, and multiplayer are not implemented yet.
- Next planned step: Chess v1 Human vs AI with a simple rule-based opponent first, starting with random legal moves or a basic material-priority move picker.
- Later TODO: Chess multiplayer over ESP-NOW, with one BitSlate as White and one as Black, synchronized moves, and reset/rematch flow.

## 2026-07-01 American States Quiz Python Prototype

- Started the BitSlate Geography app `American States Quiz`.
- Desktop Python prototype created in `src/apps/geography/us_states_quiz/`.
- Uses the USA map reference images in `src/apps/assets/images/geography/`.
- v0 uses a simplified polygon-region approach instead of one bitmap per state.
- Implemented a 15-state working subset for click detection and prompt flow.
- Correct clicks highlight states and advance the prompt; wrong clicks keep the same target.
- This prototype is meant to validate map fit, prompt flow, click-to-state detection, and color-fill feedback before any C++ or LVGL port.

## 2026-07-01 American States Quiz Generated Map Direction

- Geography work is now pivoted away from the visible-PNG overlay quiz approach.
- Added a generated USA grid/cell map asset pipeline based on `usa-map-unselected.png`.
- Preview now renders generated map data directly, without drawing the PNG as the visible map.
- Added a manual state polygon editor that traces polygons in generated-map grid coordinates.
- Initial manual tracing workflow is set up for California, Texas, and Washington.

## 2026-07-01 American States Quiz 3-State Mini Quiz

- Built the next desktop quiz step using only the traced polygons for Washington, California, and Texas.
- The visible base map comes from generated grid data, not from directly displaying the PNG alone.
- A light PNG overlay is still used for readability and alignment in this desktop phase.
- Hit detection now uses the saved traced polygons from `state_polygons.py`.
- Correct answers stay filled and the quiz advances through the 3-state prompt sequence.

## 2026-07-01 American States Quiz 20-State Tracing Set

- Returned focus to polygon authoring instead of quiz expansion.
- Increased default overlay opacity in the polygon editor to make the PNG reference easier to trace against.
- Expanded the editable tracing set to 20 states:
  - California
  - Texas
  - Washington
  - Oregon
  - Nevada
  - Arizona
  - Colorado
  - New Mexico
  - Utah
  - Idaho
  - Montana
  - Wyoming
  - Florida
  - Georgia
  - New York
  - Pennsylvania
  - Michigan
  - Illinois
  - Ohio
  - North Carolina
- Existing California, Texas, and Washington polygons remain editable and clearable.
- Editor now supports next/previous state navigation and save summaries.

## 2026-07-01 American States Quiz Color-Segmented State Masks

- Manual polygon tracing is now paused in favor of computer-generated state masks.
- Added a color-segmentation pipeline based on `usa-map-full.png`.
- The new generator maps colored states into the same generated grid coordinate system as `generated_us_map.py`.
- First sample masks are generated for:
  - California
  - Texas
  - Washington
- Output is saved as grid-cell masks, not polygons.
- This direction is a better fit for later ESP32/LVGL work because click detection can be grid-cell based instead of relying on per-state bitmaps or manual polygon outlines.

## 2026-07-01 American States Quiz Generated Mask Cleanup

- Current generated mask set is intentionally held at six states: California, Texas, Washington, South Dakota, Alaska, and Missouri.
- Boundary-constrained generated masks are working with overlay preview (`O`) and border/label preview (`B`).
- Final cleanup pass touched only Alaska, Washington, and Texas.
- Alaska now preserves a single connected filled inset region using seed `(16, 88)`, `min_hits=1`, and cleanup `(4, 2)`.
- Washington now uses seed `(15, 6)`, default `min_hits=2`, and cleanup `(6, 2)` for a slightly cleaner northwest mask.
- Texas now uses seed `(70, 83)`, `min_hits=3`, and cleanup `(5, 2)` to reduce leakage and jagged artifacts.
- California, South Dakota, and Missouri remain unchanged from the generated color-segmented masks.
- Final mask cell counts: California 595, Texas 927, Washington 227, South Dakota 268, Alaska 281, Missouri 249.
- Python mask tests and `py_compile` passed.

## 2026-07-02 American States Quiz Vector LVGL Checkpoint

- American States Quiz now uses vector-derived U.S. Census Cartographic Boundary data.
- Generated grid: `240x150`.
- Exported states: `48` contiguous states.
- Border cells: `3308`.
- LVGL renders one RGB565 image buffer in PSRAM instead of drawing many objects.
- Current map is enlarged, vertically stretched slightly, gray-filled, and drawn with white borders.
- Quiz order: Washington, California, Texas, South Dakota, Missouri.
- Touch conversion maps screen-local coordinates into the generated grid before calling the portable quiz core.
- The embedded app uses `UsStatesQuizCore` and generated `UsStatesVectorData`.
- No shapefile parser, PNG state maps, or full-color bitmap state assets are used on-device.
- Desktop/vector prototype and generated-data pipeline exist under `src/apps/geography/us_states_quiz/vector_pipeline/`.

## 2026-07-02 Phase 1 Initial Apps Checkpoint

- Flags Quiz LVGL prototype added.
- Multi-flag quiz uses converted PNG flag assets as LVGL RGB565 image descriptors.
- Four-answer touch UI works.
- Correct answer shows bright green `Correct!`.
- App advances after a non-blocking 4-second LVGL timer.
- Wrong answer shows `Try Again`.
- Current app launch path is `FlagsQuizApp::create()`.
- Previous app checkpoints include vector-based US States Quiz, Chess, calculators, and STEM simulations.

## 2026-07-02 Zoomable Geography Map Engines

- Zoomable USA map engine added with camera zoom/pan, bit-grid style rendering, capital dots/labels, and state labels.
- Zoomable WORLD map added with Natural Earth generated data, blue ocean, colored countries, bit-grid style, borders, capitals/ocean labels, and zoom/pan.
- Both maps use fixed RGB565 buffers and generated C++ data; no shapefile parsing runs on-device.

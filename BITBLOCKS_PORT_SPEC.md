# BitBlocks desktop milestone and LVGL port specification

## Purpose

The PySide6 prototype is the visual and interaction specification for a native
BitSlate implementation. The LVGL port must reproduce behavior and shared
geometry intentionally; it must not translate Qt classes line by line.

Target hardware is the existing ESP32-S3 BitSlate with a 480×320 resistive
touchscreen/stylus and LVGL 9.5. Touch is single-pointer. The established
display/touch driver, launcher, navigation manager, and GPIO mapping remain
unchanged.

## Portable block model

Rendering and LVGL ownership are separate from the data model. A workspace
block has:

```text
id              stable runtime identifier
opcode          definition key
category        Movement / Events / Control / Operators / Camera
arguments       bounded key/value strings
x, y            workspace coordinates
next_block_id   optional vertically attached successor
parent_block_id optional containing/preceding block
```

The initial device implementation may use fixed-capacity strings and vectors to
avoid hidden allocations. Palette definitions are immutable. Dragging from the
palette creates a new model; palette models are never moved into the workspace.

## Definitions and opcodes

| Category | Opcode | Shape | Display template / defaults |
|---|---|---|---|
| Movement | `motion_move` | stack | `move [10] steps` |
| Movement | `motion_turn` | stack | `turn [15] degrees` |
| Movement | `motion_goto` | stack | `go to x: [0] y: [0]` |
| Movement | `motion_direction` | stack | `set direction [90]` |
| Events | `event_start` | hat | `when [green play icon] clicked` |
| Events | `event_button` | hat | `when [button] pressed` |
| Events | `event_touch` | hat | `when screen touched` |
| Events | `event_receive` | hat | `when I receive [message]` |
| Control | `control_wait` | stack | `wait [0.5] seconds` |
| Control | `control_repeat` | C | `repeat [10] times` |
| Control | `control_forever` | C | `forever` |
| Control | `control_if` | C | `if [condition] then` |
| Operators | `operator_add` | reporter | `[0] + [0]` |
| Operators | `operator_subtract` | reporter | `[0] - [0]` |
| Operators | `operator_gt` | reporter | `[0] > [0]` |
| Operators | `operator_random` | reporter | `pick random [1] to [10]` |

Camera is a visible purple category with an empty-state message and no initial
opcodes.

## Visual system

Category colors are Movement blue `#4d69b2`, Events yellow `#e3b43e`, Control
orange `#ce762d`, Operators green `#65983b`, and Camera purple `#68459d`.
Outlines use a category-darkened color. Argument fields are cream/white with a
dark outline and centered dark text. Use the generated Monogram LVGL fonts
already in `src/assets/fonts/generated` at integer sizes.

All silhouettes use shared integer geometry:

- Stack: stepped top notch and stepped bottom connector.
- Event: aggressive tall top-left shoulder, raised plateau, staircase slope,
  and bottom connector.
- Reporter: horizontal value shape with stepped/octagonal ends.
- C-block: header, left spine, open cavity, and lower closing arm.
- Pixel-stepped outer corners and no antialiased curves.
- One closed outline per silhouette and an inset top highlight following the
  notch/hat contour.
- No external drop shadow and no painting outside the declared object bounds.

Palette and workspace must call the same geometry/renderer. Argument layout is
font-metric driven; input hitboxes, displayed fields, and text stay aligned.

## 480×320 layout

- Left sidebar: compact category selector, scrollable palette, and lower 3:2
  Output Preview.
- Right: white workspace with compact zoom-equivalent controls only if useful
  on device, and green Play/red Stop controls at its lower-right.
- Palette background is charcoal; the application surround is light gray.
- Sidebar/workspace proportions should maximize the workspace while keeping
  palette blocks usable with a stylus.

The desktop page selectors are full-screen overlays. Device selectors show four
sprite/backdrop cards per page with discrete paging and Back/Confirm controls.

## Touch and drag behavior

1. Pressing a palette block creates a temporary drag view and a new workspace
   model; the palette source remains in place.
2. The drag preview contains only the block pixels and alpha outside its
   silhouette. It is reused/moved—not duplicated on pointer move.
3. Releasing in the workspace commits the copy. Releasing elsewhere cancels it.
4. Workspace blocks remain draggable after placement.
5. Releasing a stack block close beneath another compatible block snaps its top
   connector to the prior bottom connector.
6. The models link through `next_block_id` / `parent_block_id`; moving the top
   block translates every attached successor.
7. A selected block exposes a simple touch delete action. Delete detaches links
   safely before removing the model/view.

Dirty regions must include outline/highlight pixels. Avoid debug guides,
per-move line allocation, stale caches, black preview rectangles, trails, and
objects clipped behind the sidebar.

## Sprite and backdrop system

Toolbar assets are in `src/assets/UI/bitblocks-ui`. Sprite PNGs are discovered
from `src/assets/UI/sprites`; backdrop PNGs are discovered from
`src/assets/UI/backdrops`.

- Default sprite: `capybara.png`, otherwise first available sprite.
- Default backdrop: `wilderness.png`, otherwise first available backdrop.
- Render backdrop first, filling/cropping proportionally to the 3:2 preview.
- Render the transparent sprite centered above it with nearest-neighbor scaling.
- Each green toolbar trigger expands/collapses its blue tool strip.
- Search opens its full-screen selector; Sparkle chooses a random asset; Paint
  remains a placeholder.
- Selection state stores the chosen asset path/identifier for later runtime use.

For firmware flash limits, source PNGs may require an explicit conversion and
packing pipeline before all desktop assets can ship on device. Do not embed all
large PNGs blindly.

## Lifecycle and launcher integration

BitBlocks is a leaf app registered in `app_registry`. Creation must construct a
new LVGL screen and app state. Cleanup must cancel active drag state, delete any
timers/animations, and release owned model/view memory before the screen is
deleted. GPIO20 and the central app manager return to the correct launcher.

## Deferred features

- Interpreter/execution runtime and Play/Stop behavior
- Code generation or compiler
- Variables and persistence
- Advanced C-block nesting/execution
- ESP-NOW/classroom collaboration
- Camera and Paint tools
- Animation/movement simulation
- Production asset packing and memory optimization

The first port milestone is a touch-driven editor vertical slice, not a runtime.

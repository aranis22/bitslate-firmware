# BitSlate project context

## Overview

BitSlate is a low-cost, offline classroom learning device. The current firmware milestone is a touch-first educational launcher that also supports physical buttons, category menus, and the basic interactive apps already implemented in the repository. The longer-term product is a teacher/student classroom platform with ESP-NOW messaging, orchestration, collaboration, progress, and additional learning activities.

## Hardware

- MCU/module target: ESP32-S3-WROOM-1-N16R8 with Octal PSRAM.
- Display: 480×320 TFT, initialized through `src/main.cpp` and `src/hal/display/LGFX_BitSlate.hpp`.
- Input: resistive touchscreen/stylus via the LovyanGFX touch callback; no assumptions are made about multitouch.
- Flash/PSRAM: large bitmap descriptors are stored in flash. Memory-heavy map apps allocate with PSRAM-capable heap flags and clean up when their screen is destroyed.

Physical controls are active-low with `INPUT_PULLUP` and 30 ms debounce in `src/ui/screens/button_input.cpp`:

```text
GPIO20 = Back / rotary encoder button
GPIO47 = Left
GPIO48 = Right
GPIO37 must never be used because it is reserved by the ESP32-S3-WROOM-1-N16R8 Octal PSRAM interface.
```

## Software architecture

`src/main.cpp` initializes LVGL 9.5, the 480×320 display buffers, touch input, and the central button poller. `src/ui/screens/home_screen.c` owns the Home carousel. `src/ui/screens/{physics,chemistry,math,geography}_menu.c` are visual category screens.

`src/ui/navigation/app_registry.h/.cpp` defines `app_id_t` and `app_descriptor_t` entries with display names, start callbacks, and cleanup callbacks. `src/ui/navigation/app_manager.h/.cpp` owns deferred `lv_async_call()` transitions, the active category, leaf-app state, and the back destination. Launcher code dispatches an app ID or category menu; it does not construct leaf apps directly.

`button_input.h` defines explicit `ui_state_t` values for Home, categories, leaf apps, and transitions. State changes reset pending debounce state. `app_manager.cpp` prevents double launches with `transition_pending`; it creates the destination screen and deletes the old screen outside the originating LVGL event callback.

Cleanup is app-owned. `MassesSpringsApp::destroy()` and `CollisionLabApp::destroy()` delete simulation timers. `FlagsQuizApp::destroy()` deletes the delayed advance timer. `UsStatesQuizApp::destroy()` deletes its quiz timer and frees its map buffer. World Map and Chess retain their dedicated cleanup wrappers.

## UI flow and implementation status

```text
Home
├── Chess                         connected: chess_render_smoke_app_create
├── World Map                     connected: zoomable_world_map_app_create
├── Physics
│   ├── Masses / Springs          connected: MassesSpringsApp::create
│   ├── Collision                 connected: CollisionLabApp::create
│   ├── Circuits                  connected: CircuitLabApp::create
│   ├── Waves                     inactive: no implementation
│   ├── Kinematics                inactive: no implementation
│   └── Forces                    inactive: no implementation
├── Chemistry
│   ├── Periodic Table            connected: PeriodicTableApp::create
│   └── Atom Lab                  connected: BuildAtomApp::create
├── Math
│   ├── Graphing Calculator       connected: GraphingCalculatorApp::create
│   └── Scientific Calculator     connected: ScientificCalculatorApp::create
├── Geography
│   ├── US States Quiz            connected: UsStatesQuizApp::create
│   └── World Flags Quiz          connected: FlagsQuizApp::create
└── Settings                      inactive: no implementation
```

Category cards use the shared two-card carousel in `home_screen.c`. Category menus keep the castle background/status bar and use fixed-size card descriptors. Settings remains a safe no-op Home card.

## Input behavior

Touch arrows and GPIO47/GPIO48 navigate the Home carousel only. The carousel transition lock blocks another navigation until its one-strip animation completes. Touching a category/leaf card schedules navigation through `app_manager`.

GPIO20 calls `app_exit_to_previous_menu()`: from a leaf it returns to the category that launched it; Chess and World Map return to Home; from any category it returns Home. GPIO47/GPIO48 intentionally do nothing in categories and leaf apps unless an app owns its own input. GPIO37 is never configured or read.

## Asset pipeline

Home source PNGs and generated descriptors are in `src/assets/UI/home`. The proven workflow is:

1. Export artwork at its intended on-screen dimensions.
2. Resize once during asset preparation; do not call LVGL runtime scaling for cards.
3. Generate a C descriptor with `LV_COLOR_FORMAT_RGB565`, exact width/height, and stride of `width * 2` for opaque art.
4. Declare the descriptor in `src/assets/UI/home/ui_home_assets.h` using the `ui_home_*` or category-specific naming convention.

Use `RGB565A8` only where alpha is truly required. Home cards are 220×220 and animate as a clipped horizontal strip; their viewport position/bobbing is defined in `home_screen.c`.

## App lifecycle rules

- Register every leaf app in `app_registry.cpp`; do not put leaf-app initialization in a launcher screen.
- Provide a cleanup callback whenever an app owns timers, animations, callbacks, buffers, or other dynamic resources.
- Stop timers/animations before deleting the app screen.
- Free dynamic memory only in cleanup while the app screen is being destroyed.
- Use `app_launch`, `app_exit_to_previous_menu`, or `app_exit_to_home`; navigation is deferred outside active LVGL callbacks.
- Transition locks and `button_input_set_ui_state()` prevent double launches and clear input state during screen changes.
- Keep PSRAM-safe allocation for map and other memory-heavy apps.

## Adding an app

1. Add and convert assets at final render size.
2. Implement the LVGL app screen and a cleanup function for every owned resource.
3. Add an `app_id_t` and descriptor in `src/ui/navigation/app_registry.cpp`.
4. Add the card to the relevant launcher/menu array and dispatch its ID with `app_launch`.
5. Ensure the category is the intended back destination.
6. Test repeated launch/exit cycles.
7. Check flash, RAM, heap, and PSRAM usage before publishing.

## Constraints and technical debt

- The current clean build consumes 2,945,785 / 3,145,728 flash bytes (93.6%); uncompressed RGB565 assets are the main pressure.
- Static RAM is 209,844 / 327,680 bytes (64.0%).
- Settings, Waves, Kinematics, and Forces have cards but no application implementation.
- There is no ESP-NOW connectivity, account/role system, persistence, or progress tracking.
- Interactive app-cycle testing must be performed on the target touch hardware after each substantial navigation change.

## Roadmap

1. Connectivity foundation
2. ESP-NOW messaging
3. Teacher/student roles
4. Classroom activities
5. Multiplayer collaboration
6. Save/progress system
7. Additional apps
8. Optimization and production hardening

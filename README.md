# BitSlate Firmware

BitSlate is an offline, ESP32-S3-based classroom learning device. This milestone delivers a 480×320 LVGL 9.5 launcher with animated app cards, category sub-launchers, physical controls, and a central registry for the educational apps already in the repository.

## Hardware and UI

- Target: ESP32-S3-WROOM-1-N16R8 (Octal PSRAM)
- Display: 480×320 TFT with resistive touch/stylus input
- Framework: Arduino, PlatformIO, LovyanGFX, and LVGL 9.5
- Home UI: clipped, two-card horizontal carousel with a single strip animation and card bobbing
- Controls: touch arrows or GPIO47/GPIO48 on Home; GPIO20 is Back

GPIO20 is the active-low Back/rotary button, GPIO47 is Left, and GPIO48 is Right. GPIO37 is reserved by the module PSRAM interface and must not be used.

## Current Project Stage

Completed:

- Core BitSlate launcher UI
- App-card navigation and animations
- Category sub-launchers
- Modular app registry/navigation
- Basic educational apps working
- Touch and physical-button navigation
- Safe app launch, cleanup, and back-navigation flow

Primary next work:

- ESP-NOW connectivity
- Teacher/student device roles
- Classroom orchestration and activities
- Multiplayer/local collaboration
- More educational apps and labs
- Save/progress system
- Settings implementation
- Performance and memory optimization

## Current launcher

Home contains Chess, World Map, Physics, Chemistry, Math, Geography, and Settings. Physics, Chemistry, Math, and Geography open category menus. Registered leaf apps are declared in `src/ui/navigation/app_registry.cpp` and launched through `src/ui/navigation/app_manager.cpp`.

Connected apps include Chess, World Map, Masses/Springs, Collision, Circuits, Periodic Table, Atom Lab, Graphing Calculator, Scientific Calculator, US States Quiz, and World Flags Quiz. Waves, Kinematics, Forces, and Settings intentionally remain inactive because no matching app implementation is present.

## Build and upload

From `bitslate-firmware`:

```powershell
$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe run -t clean
$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe run
$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe run -t upload --upload-port COM15
```

The current clean build uses 209,844 / 327,680 bytes of RAM (64.0%) and 2,945,785 / 3,145,728 bytes of flash (93.6%). Flash headroom is therefore limited.

## Asset convention

Launcher artwork lives in `src/assets/UI/home`. Export/resize each asset at its final display dimensions before conversion; never scale the card at runtime. Opaque cards are emitted as LVGL 9.5 `LV_COLOR_FORMAT_RGB565` C descriptors. Use `RGB565A8` only for assets that genuinely require transparency.

See [project_context.md](project_context.md) for architecture, lifecycle, app status, and contributor guidance.

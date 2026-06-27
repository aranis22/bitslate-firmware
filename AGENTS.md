# BitSlate firmware rules

This project is BitSlate, not EDOS.

BitSlate is a low-cost child-friendly embedded learning device built on ESP32-S3, touchscreen UI, local storage, and local classroom networking.

## Stack

- PlatformIO
- Arduino framework for current prototype
- LovyanGFX for display/touch backend
- LVGL for GUI, currently paused until explicitly instructed
- C++17
- ESP32-S3 target

## Hardware rules

Do not change the verified display/touch pinout unless explicitly instructed.

Verified pinout:

- TFT_MOSI = GPIO18
- TFT_MISO = GPIO6
- TFT_SCLK = GPIO17
- TFT_CS = GPIO9
- TFT_DC = GPIO8
- TFT_RST = GPIO3
- TOUCH_CS = GPIO7
- TOUCH_IRQ = -1 / not connected
- Backlight LED is tied directly to 5V

Do not remove the `LGFX_BitSlate::getTouch(...)` X-inversion wrapper.

Do not replace LovyanGFX.

Do not re-enable LVGL until explicitly instructed.

## Architecture rules

Do not collapse architecture into one giant `main.cpp`.

Keep layers separate:

- src/hal/ for hardware abstraction
- src/ui/ for LVGL screens/components
- src/apps/ for lessons, games, quizzes, coding tools
- src/core/ for app manager, navigation, storage, messaging

Do not hardcode random GPIOs inside UI code.
All pins belong in include/bitslate_config.h.

Do not introduce new display libraries unless explicitly asked.
Do not replace LVGL or LovyanGFX.

Prefer small files and testable modules.

## Build command

Use:

pio run

## Upload command

Use:

pio run -t upload

## Serial monitor

Use:

pio device monitor

## Current milestone

LovyanGFX-only ILI9488 display and XPT2046-style touch bring-up is confirmed working.

LVGL bring-up is paused until explicitly requested.

Always build before claiming success.

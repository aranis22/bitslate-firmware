# BitSlate Firmware

ESP32-S3 firmware for a low-cost offline classroom learning device.

## Current Status

LovyanGFX-only display/touch bring-up is confirmed working.

LVGL is planned, but paused until the display backend is stable.

## Current Stack

- PlatformIO
- Arduino framework
- LovyanGFX
- LVGL planned/paused

## Hardware Checkpoint

- Board: ESP32-S3
- Display: ILI9488 480x320 TFT
- Touch: XPT2046-style resistive touch on shared SPI
- Backlight: LED tied directly to 5V

## Verified Pinout

| Signal | GPIO | Notes |
| --- | ---: | --- |
| TFT_MOSI | 18 | Display SDI/DIN and touch T_DIN |
| TFT_MISO | 6 | Touch T_DO only |
| TFT_SCLK | 17 | Display SCK and touch T_CLK |
| TFT_CS | 9 | Display chip select |
| TFT_DC | 8 | Display data/command |
| TFT_RST | 3 | Display reset |
| TOUCH_CS | 7 | Touch chip select |
| TOUCH_IRQ | -1 | Not connected |

## Commands

Build:

```powershell
pio run
```

Upload:

```powershell
pio run -t upload
```

Monitor:

```powershell
pio device monitor -p COM15 -b 115200
```

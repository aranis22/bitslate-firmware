#include <Arduino.h>
#include "hal/display/LGFX_BitSlate.hpp"

static LGFX_BitSlate display;

void setup() {
Serial.begin(115200);
delay(2000);

Serial.println("BitSlate LovyanGFX confirmed-pin test");
Serial.println("before display.init");

display.init();
display.setRotation(BITSLATE_DISPLAY_ROTATION);

Serial.println("after display.init");

display.fillScreen(TFT_RED);
Serial.println("red");
delay(800);

display.fillScreen(TFT_GREEN);
Serial.println("green");
delay(800);

display.fillScreen(TFT_BLUE);
Serial.println("blue");
delay(800);

display.fillScreen(TFT_BLACK);
display.setTextColor(TFT_WHITE, TFT_BLACK);
display.setTextSize(3);
display.setCursor(30, 30);
display.println("BitSlate");
display.setTextSize(2);
display.setCursor(30, 80);
display.println("LovyanGFX OK");

display.drawRect(10, 10, 460, 300, TFT_GREEN);

Serial.println("draw complete");
}

void loop() {
  uint16_t x, y;
  if (display.getTouch(&x, &y)) {
    Serial.printf("Touch: %u, %u\n", x, y);
    display.fillCircle(x, y, 3, TFT_MAGENTA);
    delay(50);
  }
}

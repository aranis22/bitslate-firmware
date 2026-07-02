// Native build, when g++ is available:
// g++ -std=c++17 ^
//   src/apps/geography/us_states_quiz/generated/UsStatesVectorData.cpp ^
//   src/apps/geography/us_states_quiz/core/ZoomableUsMapCore.cpp ^
//   src/apps/geography/us_states_quiz/core/test_zoomable_us_map_core.cpp ^
//   -o src/apps/geography/us_states_quiz/core/test_zoomable_us_map_core.exe

#include "ZoomableUsMapCore.h"

#include "../generated/UsStatesVectorData.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>

using bitslate::geo::MapCamera;
using bitslate::geo::US_STATE_NONE;
using bitslate::geo::ZoomableMapRenderOptions;
using bitslate::geo::ZoomableUsMapCore;

namespace {

constexpr int kViewW = 480;
constexpr int kViewH = 320;

const char* const kSampleStates[] = {
    "Washington",
    "California",
    "Texas",
    "South Dakota",
    "Missouri",
};

bool expect(bool value, const char* message) {
  if (!value) {
    printf("FAIL: %s\n", message);
    return false;
  }
  return true;
}

bool nearFloat(float a, float b, float tolerance) {
  return fabsf(a - b) <= tolerance;
}

bool findCellForState(int stateId, uint16_t& outX, uint16_t& outY) {
  for (uint16_t y = 0; y < bitslate::geo::US_GRID_H; ++y) {
    for (uint16_t x = 0; x < bitslate::geo::US_GRID_W; ++x) {
      if (bitslate::geo::stateAtCell(x, y) == stateId) {
        outX = x;
        outY = y;
        return true;
      }
    }
  }
  return false;
}

void rgb565ToRgb888(uint16_t color, uint8_t& r, uint8_t& g, uint8_t& b) {
  r = static_cast<uint8_t>(((color >> 11) & 0x1F) * 255 / 31);
  g = static_cast<uint8_t>(((color >> 5) & 0x3F) * 255 / 63);
  b = static_cast<uint8_t>((color & 0x1F) * 255 / 31);
}

bool savePpmPreview(const uint16_t* buffer, int width, int height) {
  const char* path = "src/apps/geography/us_states_quiz/artifacts/zoomable_us_map_core_preview.ppm";
  FILE* file = fopen(path, "wb");
  if (file == nullptr) {
    printf("PPM preview skipped: could not open %s\n", path);
    return false;
  }

  fprintf(file, "P6\n%d %d\n255\n", width, height);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      uint8_t rgb[3] = {};
      rgb565ToRgb888(buffer[y * width + x], rgb[0], rgb[1], rgb[2]);
      fwrite(rgb, 1, 3, file);
    }
  }
  fclose(file);
  printf("PPM preview: %s\n", path);
  return true;
}

}  // namespace

int main() {
  bool ok = true;
  ZoomableUsMapCore core;

  const MapCamera& camera = core.camera();
  ok &= expect(nearFloat(camera.centerX, bitslate::geo::US_GRID_W * 0.5f, 0.001f), "default camera centerX");
  ok &= expect(nearFloat(camera.centerY, bitslate::geo::US_GRID_H * 0.5f, 0.001f), "default camera centerY");
  ok &= expect(nearFloat(camera.zoom, 1.0f, 0.001f), "default camera zoom");

  int centerGridX = -1;
  int centerGridY = -1;
  ok &= expect(core.screenToGrid(kViewW * 0.5f, kViewH * 0.5f, kViewW, kViewH, centerGridX, centerGridY), "center maps to grid");
  ok &= expect(centerGridX == bitslate::geo::US_GRID_W / 2, "center grid x");
  ok &= expect(centerGridY == bitslate::geo::US_GRID_H / 2, "center grid y");

  for (const char* stateName : kSampleStates) {
    const int stateId = bitslate::geo::findStateByName(stateName);
    ok &= expect(stateId >= 0, "sample state resolves");
    uint16_t x = 0;
    uint16_t y = 0;
    ok &= expect(findCellForState(stateId, x, y), "sample state has grid cell");
  }

  float screenX = 0.0f;
  float screenY = 0.0f;
  ok &= expect(core.gridToScreen(120.5f, 75.5f, kViewW, kViewH, screenX, screenY), "gridToScreen center visible");
  int roundTripX = -1;
  int roundTripY = -1;
  ok &= expect(core.screenToGrid(screenX, screenY, kViewW, kViewH, roundTripX, roundTripY), "screenToGrid round-trip valid");
  ok &= expect(roundTripX == 120 && roundTripY == 75, "screen/grid round-trip");

  core.zoomAtScreenPoint(kViewW * 0.5f, kViewH * 0.5f, 2.0f, kViewW, kViewH);
  ok &= expect(nearFloat(core.camera().zoom, 2.0f, 0.001f), "zoom increases");
  core.zoomAtScreenPoint(kViewW * 0.5f, kViewH * 0.5f, 0.25f, kViewW, kViewH);
  ok &= expect(nearFloat(core.camera().zoom, ZoomableUsMapCore::kMinZoom, 0.001f), "zoom clamps down");

  core.resetCamera();
  int beforeX = -1;
  int beforeY = -1;
  const float anchorX = 360.0f;
  const float anchorY = 180.0f;
  ok &= expect(core.screenToGrid(anchorX, anchorY, kViewW, kViewH, beforeX, beforeY), "anchor point starts valid");
  core.zoomAtScreenPoint(anchorX, anchorY, 3.0f, kViewW, kViewH);
  int afterX = -1;
  int afterY = -1;
  ok &= expect(core.screenToGrid(anchorX, anchorY, kViewW, kViewH, afterX, afterY), "anchor point remains valid");
  ok &= expect(abs(afterX - beforeX) <= 1 && abs(afterY - beforeY) <= 1, "zoom-at-point preserves grid point");

  const float panStartX = core.camera().centerX;
  const float panStartY = core.camera().centerY;
  core.panByPixels(24.0f, -12.0f, kViewW, kViewH);
  ok &= expect(!nearFloat(core.camera().centerX, panStartX, 0.001f), "pan changes camera centerX");
  ok &= expect(!nearFloat(core.camera().centerY, panStartY, 0.001f), "pan changes camera centerY");

  core.resetCamera();
  const int texasId = bitslate::geo::findStateByName("Texas");
  uint16_t texasX = 0;
  uint16_t texasY = 0;
  ok &= expect(findCellForState(texasId, texasX, texasY), "Texas cell found");
  ok &= expect(core.gridToScreen(texasX + 0.5f, texasY + 0.5f, kViewW, kViewH, screenX, screenY), "Texas cell visible");
  ok &= expect(core.stateAtScreen(screenX, screenY, kViewW, kViewH) == texasId, "stateAtScreen finds known cell");

  static uint16_t buffer[160 * 100];
  core.renderMapRgb565(buffer, 160, 100, ZoomableMapRenderOptions{true, true});
  core.renderMapRgb565(buffer, 160, 100, ZoomableMapRenderOptions{false, true});
  core.renderMapRgb565(buffer, 160, 100, ZoomableMapRenderOptions{true, false});
  ok &= expect(buffer[0] != 0 || buffer[159] != 0, "render wrote buffer");
  savePpmPreview(buffer, 160, 100);

  printf(ok ? "PASS\n" : "FAIL\n");
  return ok ? 0 : 1;
}

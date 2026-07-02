// Native build, when g++ is available:
// g++ -std=c++17 ^
//   src/apps/geography/us_states_quiz/generated/WorldMapData.cpp ^
//   src/apps/geography/us_states_quiz/core/ZoomableWorldMapCore.cpp ^
//   src/apps/geography/us_states_quiz/core/test_zoomable_world_map_core.cpp ^
//   -o src/apps/geography/us_states_quiz/core/test_zoomable_world_map_core.exe

#include "ZoomableWorldMapCore.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

using bitslate::geo::WORLD_COUNTRY_NONE;
using bitslate::geo::WorldMapCamera;
using bitslate::geo::WorldMapRenderOptions;
using bitslate::geo::ZoomableWorldMapCore;

namespace {

constexpr int kViewW = 480;
constexpr int kViewH = 240;

const char* const kExpectedCountries[] = {
    "India",          "China",          "United States", "Indonesia",      "Pakistan",
    "Brazil",         "Nigeria",        "Bangladesh",    "Russia",         "Mexico",
    "Japan",          "Ethiopia",       "Philippines",   "Egypt",          "Vietnam",
    "DR Congo",       "Turkey",         "Iran",          "Germany",        "Thailand",
    "United Kingdom", "France",         "Italy",         "South Africa",   "Tanzania",
    "Myanmar",        "Kenya",          "South Korea",   "Colombia",       "Spain",
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

bool findCellForCountry(int countryId, uint16_t& outX, uint16_t& outY) {
  for (uint16_t y = 0; y < bitslate::geo::WORLD_GRID_H; ++y) {
    for (uint16_t x = 0; x < bitslate::geo::WORLD_GRID_W; ++x) {
      if (bitslate::geo::worldCountryAtCell(x, y) == countryId) {
        outX = x;
        outY = y;
        return true;
      }
    }
  }
  return false;
}

bool countryAtKnownCell(ZoomableWorldMapCore& core, const char* countryName) {
  const int countryId = bitslate::geo::findWorldCountryByName(countryName);
  uint16_t cellX = 0;
  uint16_t cellY = 0;
  float screenX = 0.0f;
  float screenY = 0.0f;
  return countryId >= 0 && findCellForCountry(countryId, cellX, cellY) &&
         core.gridToScreen(cellX + 0.5f, cellY + 0.5f, kViewW, kViewH, screenX, screenY) &&
         core.countryAtScreen(screenX, screenY, kViewW, kViewH) == countryId;
}

}  // namespace

int main() {
  bool ok = true;
  ZoomableWorldMapCore core;

  ok &= expect(bitslate::geo::WORLD_GRID_W == 360, "world grid width");
  ok &= expect(bitslate::geo::WORLD_GRID_H == 180, "world grid height");
  ok &= expect(bitslate::geo::WORLD_COUNTRY_COUNT == 30, "world country count");
  ok &= expect(bitslate::geo::WORLD_BORDER_CELL_COUNT > 0, "world border cells exist");

  for (const char* countryName : kExpectedCountries) {
    const int countryId = bitslate::geo::findWorldCountryByName(countryName);
    ok &= expect(countryId >= 0, "expected country resolves");
    uint16_t x = 0;
    uint16_t y = 0;
    ok &= expect(findCellForCountry(countryId, x, y), "expected country has grid cell");
  }

  ok &= expect(core.capitalMarkerCount() == 30, "capital marker count");
  ok &= expect(core.countryLabelPointCount() == 30, "country label count");
  ok &= expect(core.oceanLabelCount() == 5, "ocean label count");

  const WorldMapCamera& camera = core.camera();
  ok &= expect(nearFloat(camera.centerX, bitslate::geo::WORLD_GRID_W * 0.5f, 0.001f), "default camera centerX");
  ok &= expect(nearFloat(camera.centerY, bitslate::geo::WORLD_GRID_H * 0.5f, 0.001f), "default camera centerY");
  ok &= expect(nearFloat(camera.zoom, 1.0f, 0.001f), "default camera zoom");

  float screenX = 0.0f;
  float screenY = 0.0f;
  ok &= expect(core.gridToScreen(180.5f, 90.5f, kViewW, kViewH, screenX, screenY), "gridToScreen center visible");
  int roundTripX = -1;
  int roundTripY = -1;
  ok &= expect(core.screenToGrid(screenX, screenY, kViewW, kViewH, roundTripX, roundTripY), "screenToGrid round-trip valid");
  ok &= expect(roundTripX == 180 && roundTripY == 90, "screen/grid round-trip");

  int beforeX = -1;
  int beforeY = -1;
  const float anchorX = 330.0f;
  const float anchorY = 120.0f;
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
  ok &= expect(countryAtKnownCell(core, "India"), "countryAtScreen finds India");
  ok &= expect(countryAtKnownCell(core, "United States"), "countryAtScreen finds United States");
  ok &= expect(countryAtKnownCell(core, "Brazil"), "countryAtScreen finds Brazil");
  ok &= expect(countryAtKnownCell(core, "China"), "countryAtScreen finds China");

  static uint16_t buffer[160 * 80];
  core.renderMapRgb565(buffer, 160, 80, WorldMapRenderOptions{true, true});
  core.renderMapRgb565(buffer, 160, 80, WorldMapRenderOptions{false, true});
  core.renderMapRgb565(buffer, 160, 80, WorldMapRenderOptions{true, false});
  ok &= expect(buffer[0] != 0 || buffer[159] != 0, "render wrote buffer");

  printf(ok ? "PASS\n" : "FAIL\n");
  return ok ? 0 : 1;
}

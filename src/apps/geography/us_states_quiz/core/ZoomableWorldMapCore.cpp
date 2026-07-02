#include "ZoomableWorldMapCore.h"

#include <math.h>

namespace bitslate::geo {
namespace {

constexpr uint16_t kOceanBackground = 0x21AD;  // #27346D
constexpr uint16_t kBitGrid = 0x4208;
constexpr uint16_t kBorder = 0xC638;

constexpr uint16_t kMutedPalette[] = {
    0x9D70,
    0xC50B,
    0x8D56,
    0xB430,
    0xA4D7,
    0x95B5,
    0xC5EC,
    0x95CE,
    0xB515,
    0x8CF8,
    0xAD75,
    0xBCEA,
};

float clampFloat(float value, float minValue, float maxValue) {
  if (value < minValue) {
    return minValue;
  }
  if (value > maxValue) {
    return maxValue;
  }
  return value;
}

uint8_t cellCountryAt(int x, int y) {
  if (x < 0 || y < 0 || x >= WORLD_GRID_W || y >= WORLD_GRID_H) {
    return WORLD_COUNTRY_NONE;
  }
  return WORLD_COUNTRY_ID_GRID[y * WORLD_GRID_W + x];
}

void fillRectClipped(uint16_t* out, int viewW, int viewH, int x0, int y0, int x1, int y1, uint16_t color) {
  if (out == nullptr || viewW <= 0 || viewH <= 0) {
    return;
  }
  if (x0 > x1) {
    const int temp = x0;
    x0 = x1;
    x1 = temp;
  }
  if (y0 > y1) {
    const int temp = y0;
    y0 = y1;
    y1 = temp;
  }
  if (x1 < 0 || y1 < 0 || x0 >= viewW || y0 >= viewH) {
    return;
  }
  if (x0 < 0) {
    x0 = 0;
  }
  if (y0 < 0) {
    y0 = 0;
  }
  if (x1 >= viewW) {
    x1 = viewW - 1;
  }
  if (y1 >= viewH) {
    y1 = viewH - 1;
  }

  for (int y = y0; y <= y1; ++y) {
    uint16_t* row = out + y * viewW;
    for (int x = x0; x <= x1; ++x) {
      row[x] = color;
    }
  }
}

void setPixelClipped(uint16_t* out, int viewW, int viewH, int x, int y, uint16_t color) {
  if (out == nullptr || x < 0 || y < 0 || x >= viewW || y >= viewH) {
    return;
  }
  out[y * viewW + x] = color;
}

}  // namespace

ZoomableWorldMapCore::ZoomableWorldMapCore() {
  resetCamera();
}

void ZoomableWorldMapCore::resetCamera() {
  camera_.centerX = static_cast<float>(WORLD_GRID_W) * 0.5f;
  camera_.centerY = static_cast<float>(WORLD_GRID_H) * 0.5f;
  camera_.zoom = 1.0f;
}

const WorldCapitalMarker* ZoomableWorldMapCore::capitalMarkers() const {
  return WORLD_CAPITAL_MARKERS;
}

uint8_t ZoomableWorldMapCore::capitalMarkerCount() const {
  return WORLD_CAPITAL_MARKER_COUNT;
}

const WorldLabelPoint* ZoomableWorldMapCore::countryLabelPoints() const {
  return WORLD_COUNTRY_LABEL_POINTS;
}

uint8_t ZoomableWorldMapCore::countryLabelPointCount() const {
  return WORLD_COUNTRY_LABEL_COUNT;
}

const WorldOceanLabel* ZoomableWorldMapCore::oceanLabels() const {
  return WORLD_OCEAN_LABELS;
}

uint8_t ZoomableWorldMapCore::oceanLabelCount() const {
  return WORLD_OCEAN_LABEL_COUNT;
}

float ZoomableWorldMapCore::baseScale(int viewW, int viewH) const {
  if (viewW <= 0 || viewH <= 0) {
    return 1.0f;
  }
  const float sx = static_cast<float>(viewW) / static_cast<float>(WORLD_GRID_W);
  const float sy = static_cast<float>(viewH) / static_cast<float>(WORLD_GRID_H);
  return sx < sy ? sx : sy;
}

bool ZoomableWorldMapCore::screenToGridFloat(
    float screenX,
    float screenY,
    int viewW,
    int viewH,
    float& gridX,
    float& gridY
) const {
  if (viewW <= 0 || viewH <= 0) {
    return false;
  }
  const float scale = baseScale(viewW, viewH) * camera_.zoom;
  if (scale <= 0.0f) {
    return false;
  }
  gridX = (screenX - static_cast<float>(viewW) * 0.5f) / scale + camera_.centerX;
  gridY = (screenY - static_cast<float>(viewH) * 0.5f) / scale + camera_.centerY;
  return true;
}

bool ZoomableWorldMapCore::screenToGrid(float screenX, float screenY, int viewW, int viewH, int& gridX, int& gridY)
    const {
  float gx = 0.0f;
  float gy = 0.0f;
  if (!screenToGridFloat(screenX, screenY, viewW, viewH, gx, gy)) {
    return false;
  }
  gridX = static_cast<int>(floorf(gx));
  gridY = static_cast<int>(floorf(gy));
  return gridX >= 0 && gridY >= 0 && gridX < WORLD_GRID_W && gridY < WORLD_GRID_H;
}

bool ZoomableWorldMapCore::gridToScreen(
    float gridX,
    float gridY,
    int viewW,
    int viewH,
    float& screenX,
    float& screenY
) const {
  if (viewW <= 0 || viewH <= 0) {
    return false;
  }
  const float scale = baseScale(viewW, viewH) * camera_.zoom;
  screenX = (gridX - camera_.centerX) * scale + static_cast<float>(viewW) * 0.5f;
  screenY = (gridY - camera_.centerY) * scale + static_cast<float>(viewH) * 0.5f;
  return screenX >= 0.0f && screenY >= 0.0f && screenX < static_cast<float>(viewW) &&
         screenY < static_cast<float>(viewH);
}

void ZoomableWorldMapCore::zoomAtScreenPoint(float screenX, float screenY, float factor, int viewW, int viewH) {
  float beforeX = 0.0f;
  float beforeY = 0.0f;
  if (!screenToGridFloat(screenX, screenY, viewW, viewH, beforeX, beforeY)) {
    return;
  }

  camera_.zoom = clampFloat(camera_.zoom * factor, kMinZoom, kMaxZoom);

  float afterX = 0.0f;
  float afterY = 0.0f;
  if (screenToGridFloat(screenX, screenY, viewW, viewH, afterX, afterY)) {
    camera_.centerX += beforeX - afterX;
    camera_.centerY += beforeY - afterY;
  }
  clampCamera(viewW, viewH);
}

void ZoomableWorldMapCore::panByPixels(float dx, float dy, int viewW, int viewH) {
  const float scale = baseScale(viewW, viewH) * camera_.zoom;
  if (scale <= 0.0f) {
    return;
  }
  camera_.centerX -= dx / scale;
  camera_.centerY -= dy / scale;
  clampCamera(viewW, viewH);
}

void ZoomableWorldMapCore::clampCamera(int viewW, int viewH) {
  const float scale = baseScale(viewW, viewH) * camera_.zoom;
  if (scale <= 0.0f) {
    resetCamera();
    return;
  }

  const float halfVisibleW = static_cast<float>(viewW) * 0.5f / scale;
  const float halfVisibleH = static_cast<float>(viewH) * 0.5f / scale;
  const float minCenterX = halfVisibleW;
  const float maxCenterX = static_cast<float>(WORLD_GRID_W) - halfVisibleW;
  const float minCenterY = halfVisibleH;
  const float maxCenterY = static_cast<float>(WORLD_GRID_H) - halfVisibleH;

  camera_.zoom = clampFloat(camera_.zoom, kMinZoom, kMaxZoom);
  camera_.centerX = minCenterX <= maxCenterX ? clampFloat(camera_.centerX, minCenterX, maxCenterX)
                                             : static_cast<float>(WORLD_GRID_W) * 0.5f;
  camera_.centerY = minCenterY <= maxCenterY ? clampFloat(camera_.centerY, minCenterY, maxCenterY)
                                             : static_cast<float>(WORLD_GRID_H) * 0.5f;
}

uint8_t ZoomableWorldMapCore::countryAtScreen(float screenX, float screenY, int viewW, int viewH) const {
  int gridX = 0;
  int gridY = 0;
  if (!screenToGrid(screenX, screenY, viewW, viewH, gridX, gridY)) {
    return WORLD_COUNTRY_NONE;
  }
  return cellCountryAt(gridX, gridY);
}

void ZoomableWorldMapCore::renderMapRgb565(
    uint16_t* out,
    int viewW,
    int viewH,
    const WorldMapRenderOptions& options
) const {
  if (out == nullptr || viewW <= 0 || viewH <= 0) {
    return;
  }

  const float scale = baseScale(viewW, viewH) * camera_.zoom;
  const float gridLineWidth = scale > 0.0f ? 1.0f / scale : 0.0f;

  for (int y = 0; y < viewH; ++y) {
    for (int x = 0; x < viewW; ++x) {
      float gridX = 0.0f;
      float gridY = 0.0f;
      uint16_t color = kOceanBackground;
      if (screenToGridFloat(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f, viewW, viewH, gridX, gridY)) {
        const int cellX = static_cast<int>(floorf(gridX));
        const int cellY = static_cast<int>(floorf(gridY));
        const uint8_t countryId = cellCountryAt(cellX, cellY);
        if (countryId != WORLD_COUNTRY_NONE) {
          color = colorForWorldCountry(countryId);
          if (options.showBitGrid) {
            const float fracX = gridX - floorf(gridX);
            const float fracY = gridY - floorf(gridY);
            if (fracX < gridLineWidth || fracY < gridLineWidth) {
              color = kBitGrid;
            }
          }
        }
      }
      out[y * viewW + x] = color;
    }
  }

  if (!options.showBorders) {
    return;
  }

  for (uint16_t i = 0; i < WORLD_BORDER_CELL_COUNT; ++i) {
    const WorldMapCell& cell = WORLD_BORDER_CELLS[i];
    float sx = 0.0f;
    float sy = 0.0f;
    gridToScreen(static_cast<float>(cell.x) + 0.5f, static_cast<float>(cell.y) + 0.5f, viewW, viewH, sx, sy);
    setPixelClipped(out, viewW, viewH, static_cast<int>(floorf(sx)), static_cast<int>(floorf(sy)), kBorder);
  }
}

uint16_t colorForWorldCountry(uint8_t countryId) {
  if (countryId >= WORLD_COUNTRY_COUNT) {
    return kOceanBackground;
  }
  const uint8_t paletteCount = static_cast<uint8_t>(sizeof(kMutedPalette) / sizeof(kMutedPalette[0]));
  return kMutedPalette[(countryId * 7U + 3U) % paletteCount];
}

}  // namespace bitslate::geo

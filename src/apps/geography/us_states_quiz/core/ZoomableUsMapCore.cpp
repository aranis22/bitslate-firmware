#include "ZoomableUsMapCore.h"

#include "../generated/UsStatesVectorData.h"

#include <math.h>

namespace bitslate::geo {
namespace {

constexpr uint16_t kBackground = 0x112A;  // #172554
constexpr uint16_t kBitGrid = 0x4208;     // subtle dark gray
constexpr uint16_t kBorder = 0xFFFF;

constexpr uint16_t kMutedPalette[] = {
    0x9D70,  // sage
    0xC50B,  // ochre
    0x8D56,  // blue gray
    0xB430,  // clay
    0xA4D7,  // lavender gray
    0x95B5,  // teal gray
    0xC5EC,  // muted yellow
    0x95CE,  // olive
    0xB515,  // rose gray
    0x8CF8,  // periwinkle
    0xAD75,  // warm gray
    0xBCEA,  // tan
};

constexpr CapitalMarker kCapitalMarkers[] = {
    {"New York", "Albany", 210.845f, 50.874f},
    {"Texas", "Austin", 111.619f, 102.108f},
    {"Wyoming", "Cheyenne", 82.343f, 57.131f},
    {"Kansas", "Topeka", 120.116f, 65.788f},
    {"New Jersey", "Trenton", 206.763f, 60.934f},
    {"Idaho", "Boise", 35.259f, 46.893f},
    {"Wisconsin", "Madison", 146.126f, 49.135f},
    {"Connecticut", "Hartford", 215.275f, 54.551f},
    {"Michigan", "Lansing", 166.171f, 50.543f},
    {"Montana", "Helena", 52.481f, 34.590f},
    {"Arizona", "Phoenix", 52.336f, 88.949f},
    {"New Mexico", "Santa Fe", 77.720f, 79.688f},
    {"Oregon", "Salem", 6.994f, 41.400f},
    {"Washington", "Olympia", 7.550f, 32.733f},
    {"California", "Sacramento", 13.367f, 67.714f},
    {"Tennessee", "Nashville", 156.963f, 77.721f},
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

uint8_t cellStateAt(int x, int y) {
  if (x < 0 || y < 0 || x >= US_GRID_W || y >= US_GRID_H) {
    return US_STATE_NONE;
  }
  return US_STATE_ID_GRID[y * US_GRID_W + x];
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

}  // namespace

ZoomableUsMapCore::ZoomableUsMapCore() {
  resetCamera();
  computeStateLabelPoints();
}

void ZoomableUsMapCore::resetCamera() {
  camera_.centerX = static_cast<float>(US_GRID_W) * 0.5f;
  camera_.centerY = static_cast<float>(US_GRID_H) * 0.5f;
  camera_.zoom = 1.0f;
}

const CapitalMarker* ZoomableUsMapCore::capitalMarkers() const {
  return kCapitalMarkers;
}

uint8_t ZoomableUsMapCore::capitalMarkerCount() const {
  return static_cast<uint8_t>(sizeof(kCapitalMarkers) / sizeof(kCapitalMarkers[0]));
}

void ZoomableUsMapCore::computeStateLabelPoints() {
  float sumX[US_STATE_COUNT] = {};
  float sumY[US_STATE_COUNT] = {};
  uint16_t counts[US_STATE_COUNT] = {};

  for (uint16_t y = 0; y < US_GRID_H; ++y) {
    for (uint16_t x = 0; x < US_GRID_W; ++x) {
      const uint8_t stateId = cellStateAt(x, y);
      if (stateId >= US_STATE_COUNT) {
        continue;
      }
      sumX[stateId] += static_cast<float>(x) + 0.5f;
      sumY[stateId] += static_cast<float>(y) + 0.5f;
      ++counts[stateId];
    }
  }

  stateLabelCount_ = 0;
  for (uint8_t stateId = 0; stateId < US_STATE_COUNT; ++stateId) {
    if (counts[stateId] == 0) {
      continue;
    }

    const float centroidX = sumX[stateId] / static_cast<float>(counts[stateId]);
    const float centroidY = sumY[stateId] / static_cast<float>(counts[stateId]);
    float bestX = centroidX;
    float bestY = centroidY;
    float bestDist = 1000000.0f;

    for (uint16_t y = 0; y < US_GRID_H; ++y) {
      for (uint16_t x = 0; x < US_GRID_W; ++x) {
        if (cellStateAt(x, y) != stateId) {
          continue;
        }
        const float cellX = static_cast<float>(x) + 0.5f;
        const float cellY = static_cast<float>(y) + 0.5f;
        const float dx = cellX - centroidX;
        const float dy = cellY - centroidY;
        const float dist = dx * dx + dy * dy;
        if (dist < bestDist) {
          bestDist = dist;
          bestX = cellX;
          bestY = cellY;
        }
      }
    }

    stateLabels_[stateLabelCount_] = {
        stateId,
        US_STATES[stateId].name,
        bestX,
        bestY,
    };
    ++stateLabelCount_;
  }
}

float ZoomableUsMapCore::baseScale(int viewW, int viewH) const {
  if (viewW <= 0 || viewH <= 0) {
    return 1.0f;
  }
  const float sx = static_cast<float>(viewW) / static_cast<float>(US_GRID_W);
  const float sy = static_cast<float>(viewH) / static_cast<float>(US_GRID_H);
  return sx < sy ? sx : sy;
}

bool ZoomableUsMapCore::screenToGridFloat(
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

bool ZoomableUsMapCore::screenToGrid(float screenX, float screenY, int viewW, int viewH, int& gridX, int& gridY) const {
  float gx = 0.0f;
  float gy = 0.0f;
  if (!screenToGridFloat(screenX, screenY, viewW, viewH, gx, gy)) {
    return false;
  }
  gridX = static_cast<int>(floorf(gx));
  gridY = static_cast<int>(floorf(gy));
  return gridX >= 0 && gridY >= 0 && gridX < US_GRID_W && gridY < US_GRID_H;
}

bool ZoomableUsMapCore::gridToScreen(float gridX, float gridY, int viewW, int viewH, float& screenX, float& screenY)
    const {
  if (viewW <= 0 || viewH <= 0) {
    return false;
  }
  const float scale = baseScale(viewW, viewH) * camera_.zoom;
  screenX = (gridX - camera_.centerX) * scale + static_cast<float>(viewW) * 0.5f;
  screenY = (gridY - camera_.centerY) * scale + static_cast<float>(viewH) * 0.5f;
  return screenX >= 0.0f && screenY >= 0.0f && screenX < static_cast<float>(viewW) &&
         screenY < static_cast<float>(viewH);
}

void ZoomableUsMapCore::zoomAtScreenPoint(float screenX, float screenY, float factor, int viewW, int viewH) {
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

void ZoomableUsMapCore::panByPixels(float dx, float dy, int viewW, int viewH) {
  const float scale = baseScale(viewW, viewH) * camera_.zoom;
  if (scale <= 0.0f) {
    return;
  }
  camera_.centerX -= dx / scale;
  camera_.centerY -= dy / scale;
  clampCamera(viewW, viewH);
}

void ZoomableUsMapCore::clampCamera(int viewW, int viewH) {
  const float scale = baseScale(viewW, viewH) * camera_.zoom;
  if (scale <= 0.0f) {
    resetCamera();
    return;
  }

  const float halfVisibleW = static_cast<float>(viewW) * 0.5f / scale;
  const float halfVisibleH = static_cast<float>(viewH) * 0.5f / scale;
  const float minCenterX = halfVisibleW;
  const float maxCenterX = static_cast<float>(US_GRID_W) - halfVisibleW;
  const float minCenterY = halfVisibleH;
  const float maxCenterY = static_cast<float>(US_GRID_H) - halfVisibleH;

  camera_.zoom = clampFloat(camera_.zoom, kMinZoom, kMaxZoom);
  camera_.centerX = minCenterX <= maxCenterX ? clampFloat(camera_.centerX, minCenterX, maxCenterX)
                                             : static_cast<float>(US_GRID_W) * 0.5f;
  camera_.centerY = minCenterY <= maxCenterY ? clampFloat(camera_.centerY, minCenterY, maxCenterY)
                                             : static_cast<float>(US_GRID_H) * 0.5f;
}

uint8_t ZoomableUsMapCore::stateAtScreen(float screenX, float screenY, int viewW, int viewH) const {
  int gridX = 0;
  int gridY = 0;
  if (!screenToGrid(screenX, screenY, viewW, viewH, gridX, gridY)) {
    return US_STATE_NONE;
  }
  return cellStateAt(gridX, gridY);
}

void ZoomableUsMapCore::renderMapRgb565(
    uint16_t* out,
    int viewW,
    int viewH,
    const ZoomableMapRenderOptions& options
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
      uint16_t color = kBackground;
      if (screenToGridFloat(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f, viewW, viewH, gridX, gridY)) {
        const int cellX = static_cast<int>(floorf(gridX));
        const int cellY = static_cast<int>(floorf(gridY));
        const uint8_t stateId = cellStateAt(cellX, cellY);
        if (stateId != US_STATE_NONE) {
          color = colorForState(stateId);
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

  for (uint16_t i = 0; i < US_BORDER_CELL_COUNT; ++i) {
    const GeoCell& cell = US_BORDER_CELLS[i];
    float x0 = 0.0f;
    float y0 = 0.0f;
    float x1 = 0.0f;
    float y1 = 0.0f;
    gridToScreen(static_cast<float>(cell.x), static_cast<float>(cell.y), viewW, viewH, x0, y0);
    gridToScreen(static_cast<float>(cell.x) + 1.0f, static_cast<float>(cell.y) + 1.0f, viewW, viewH, x1, y1);

    int ix0 = static_cast<int>(floorf(x0));
    int iy0 = static_cast<int>(floorf(y0));
    int ix1 = static_cast<int>(ceilf(x1)) - 1;
    int iy1 = static_cast<int>(ceilf(y1)) - 1;
    fillRectClipped(out, viewW, viewH, ix0, iy0, ix1, iy1, kBorder);
  }
}

uint16_t colorForState(uint8_t stateId) {
  if (stateId >= US_STATE_COUNT) {
    return kBackground;
  }
  const uint8_t paletteCount = static_cast<uint8_t>(sizeof(kMutedPalette) / sizeof(kMutedPalette[0]));
  return kMutedPalette[(stateId * 7U + 3U) % paletteCount];
}

}  // namespace bitslate::geo

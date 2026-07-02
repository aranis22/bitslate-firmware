#pragma once

#include <stdint.h>

#include "../generated/WorldMapData.h"

namespace bitslate::geo {

struct WorldMapCamera {
  float centerX;
  float centerY;
  float zoom;
};

struct WorldMapRenderOptions {
  bool showBitGrid;
  bool showBorders;
};

class ZoomableWorldMapCore {
 public:
  static constexpr float kMinZoom = 1.0f;
  static constexpr float kMaxZoom = 12.0f;
  static constexpr float kOceanLabelZoom = 1.0f;
  static constexpr float kHugeCountryLabelZoom = 2.2f;
  static constexpr float kLargeCountryLabelZoom = 3.0f;
  static constexpr float kMediumCountryLabelZoom = 4.0f;
  static constexpr float kSmallCountryLabelZoom = 5.0f;
  static constexpr float kCapitalLabelZoom = 5.0f;

  ZoomableWorldMapCore();

  const WorldMapCamera& camera() const { return camera_; }
  float zoom() const { return camera_.zoom; }

  void resetCamera();
  void zoomAtScreenPoint(float screenX, float screenY, float factor, int viewW, int viewH);
  void panByPixels(float dx, float dy, int viewW, int viewH);
  bool screenToGrid(float screenX, float screenY, int viewW, int viewH, int& gridX, int& gridY) const;
  bool gridToScreen(float gridX, float gridY, int viewW, int viewH, float& screenX, float& screenY) const;
  uint8_t countryAtScreen(float screenX, float screenY, int viewW, int viewH) const;

  void renderMapRgb565(uint16_t* out, int viewW, int viewH, const WorldMapRenderOptions& options) const;

  const WorldCapitalMarker* capitalMarkers() const;
  uint8_t capitalMarkerCount() const;
  const WorldLabelPoint* countryLabelPoints() const;
  uint8_t countryLabelPointCount() const;
  const WorldOceanLabel* oceanLabels() const;
  uint8_t oceanLabelCount() const;

 private:
  WorldMapCamera camera_{};

  float baseScale(int viewW, int viewH) const;
  bool screenToGridFloat(float screenX, float screenY, int viewW, int viewH, float& gridX, float& gridY) const;
  void clampCamera(int viewW, int viewH);
};

uint16_t colorForWorldCountry(uint8_t countryId);

}  // namespace bitslate::geo

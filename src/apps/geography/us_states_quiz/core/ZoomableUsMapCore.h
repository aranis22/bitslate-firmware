#pragma once

#include <stdint.h>

namespace bitslate::geo {

struct CapitalMarker {
  const char* stateName;
  const char* capitalName;
  float gridX;
  float gridY;
};

struct StateLabelPoint {
  uint8_t stateId;
  const char* stateName;
  float gridX;
  float gridY;
};

struct MapCamera {
  float centerX;
  float centerY;
  float zoom;
};

struct ZoomableMapRenderOptions {
  bool showBitGrid;
  bool showBorders;
};

class ZoomableUsMapCore {
 public:
  static constexpr float kMinZoom = 1.0f;
  static constexpr float kMaxZoom = 8.0f;
  static constexpr float kStateLabelZoom = 1.7f;
  static constexpr float kCapitalLabelZoom = 2.0f;

  ZoomableUsMapCore();

  const MapCamera& camera() const { return camera_; }
  float zoom() const { return camera_.zoom; }

  void resetCamera();
  void zoomAtScreenPoint(float screenX, float screenY, float factor, int viewW, int viewH);
  void panByPixels(float dx, float dy, int viewW, int viewH);
  bool screenToGrid(float screenX, float screenY, int viewW, int viewH, int& gridX, int& gridY) const;
  bool gridToScreen(float gridX, float gridY, int viewW, int viewH, float& screenX, float& screenY) const;
  uint8_t stateAtScreen(float screenX, float screenY, int viewW, int viewH) const;

  void renderMapRgb565(uint16_t* out, int viewW, int viewH, const ZoomableMapRenderOptions& options) const;

  const CapitalMarker* capitalMarkers() const;
  uint8_t capitalMarkerCount() const;
  const StateLabelPoint* stateLabelPoints() const { return stateLabels_; }
  uint8_t stateLabelPointCount() const { return stateLabelCount_; }

 private:
  MapCamera camera_{};
  StateLabelPoint stateLabels_[48] = {};
  uint8_t stateLabelCount_ = 0;

  float baseScale(int viewW, int viewH) const;
  bool screenToGridFloat(float screenX, float screenY, int viewW, int viewH, float& gridX, float& gridY) const;
  void clampCamera(int viewW, int viewH);
  void computeStateLabelPoints();
};

uint16_t colorForState(uint8_t stateId);

}  // namespace bitslate::geo

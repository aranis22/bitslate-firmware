#include "ZoomableWorldMapApp.h"

#include <Arduino.h>
#include <cstdio>

#include <esp_heap_caps.h>
#include <lvgl.h>

#include "../core/ZoomableWorldMapCore.h"
#include "../generated/WorldMapData.h"

LV_FONT_DECLARE(monogram_16);
LV_FONT_DECLARE(monogram_20);
LV_FONT_DECLARE(monogram_24);

namespace {

constexpr int SCREEN_W = 480;
constexpr int SCREEN_H = 320;
constexpr int MAP_W = 470;
constexpr int MAP_H = 270;
constexpr int MAP_BYTES = MAP_W * MAP_H * 2;
constexpr int MAP_X = (SCREEN_W - MAP_W) / 2;
constexpr int MAP_Y = 6;
constexpr int BUTTON_Y = 292;
constexpr int BUTTON_H = 24;
constexpr int SMALL_BUTTON_W = 42;
constexpr int RESET_BUTTON_W = 78;
constexpr int GRID_BUTTON_W = 70;
constexpr int LABELS_BUTTON_W = 78;
constexpr int DRAG_TAP_THRESHOLD = 7;
constexpr int CAPITAL_DOT_SIZE = 6;
constexpr int CAPITAL_LABEL_W = 78;
constexpr int CAPITAL_LABEL_H = 14;
constexpr int COUNTRY_LABEL_W = 96;
constexpr int COUNTRY_LABEL_H = 14;
constexpr int OCEAN_LABEL_W = 112;
constexpr int OCEAN_LABEL_H = 14;

bitslate::geo::ZoomableWorldMapCore mapCore;
bitslate::geo::WorldMapRenderOptions renderOptions{true, true};
bool labelsEnabled = true;

lv_obj_t* titleLabel = nullptr;
lv_obj_t* statusLabel = nullptr;
lv_obj_t* mapArea = nullptr;
lv_obj_t* mapImage = nullptr;
lv_obj_t* gridButtonLabel = nullptr;
lv_obj_t* labelsButtonLabel = nullptr;
lv_obj_t* capitalDots[bitslate::geo::WORLD_CAPITAL_MARKER_COUNT] = {};
lv_obj_t* capitalLabels[bitslate::geo::WORLD_CAPITAL_MARKER_COUNT] = {};
lv_obj_t* countryLabels[bitslate::geo::WORLD_COUNTRY_LABEL_COUNT] = {};
lv_obj_t* oceanLabels[bitslate::geo::WORLD_OCEAN_LABEL_COUNT] = {};
uint16_t* mapPixels = nullptr;
lv_image_dsc_t mapImageDsc = {};
const char* bufferStatus = "Map buffer pending";

bool pointerDown = false;
bool dragging = false;
lv_point_t pressPoint = {};
lv_point_t lastPoint = {};
int dragDistance = 0;

lv_color_t color(unsigned int hex) {
  return lv_color_hex(hex);
}

void clearInteraction(lv_obj_t* obj) {
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

void hideObj(lv_obj_t* obj, bool hide) {
  if (obj == nullptr) {
    return;
  }
  if (hide) {
    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
  }
}

void styleBox(lv_obj_t* obj, unsigned int bg, unsigned int border, int borderWidth, int radius) {
  lv_obj_remove_style_all(obj);
  lv_obj_set_style_bg_color(obj, color(bg), 0);
  lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(obj, color(border), 0);
  lv_obj_set_style_border_width(obj, borderWidth, 0);
  lv_obj_set_style_radius(obj, radius, 0);
  lv_obj_set_style_shadow_width(obj, 0, 0);
  lv_obj_set_style_outline_width(obj, 0, 0);
  lv_obj_set_style_pad_all(obj, 0, 0);
  clearInteraction(obj);
}

lv_obj_t* makeLabel(lv_obj_t* parent, const char* text, const lv_font_t* font, unsigned int textColor) {
  lv_obj_t* label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_obj_set_style_text_font(label, font, 0);
  lv_obj_set_style_text_color(label, color(textColor), 0);
  lv_obj_set_style_text_letter_space(label, 0, 0);
  clearInteraction(label);
  return label;
}

void styleOverlayLabel(lv_obj_t* label, unsigned int textColor) {
  lv_obj_set_style_text_font(label, &monogram_16, 0);
  lv_obj_set_style_text_color(label, color(textColor), 0);
  lv_obj_set_style_bg_opa(label, LV_OPA_TRANSP, 0);
  lv_obj_set_style_pad_hor(label, 1, 0);
  lv_obj_set_style_pad_ver(label, 0, 0);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
  clearInteraction(label);
}

void setStatus(const char* text) {
  if (statusLabel != nullptr) {
    lv_label_set_text(statusLabel, text);
  }
}

const char* countryName(uint8_t countryId) {
  if (countryId >= bitslate::geo::WORLD_COUNTRY_COUNT) {
    return nullptr;
  }
  return bitslate::geo::WORLD_COUNTRIES[countryId].name;
}

bool screenPointVisible(float screenX, float screenY, int margin) {
  return screenX >= -margin && screenY >= -margin && screenX < MAP_W + margin && screenY < MAP_H + margin;
}

float countryLabelZoomThreshold(uint16_t cellCount) {
  if (cellCount >= 1200) {
    return bitslate::geo::ZoomableWorldMapCore::kHugeCountryLabelZoom;
  }
  if (cellCount >= 600) {
    return bitslate::geo::ZoomableWorldMapCore::kLargeCountryLabelZoom;
  }
  if (cellCount >= 150) {
    return bitslate::geo::ZoomableWorldMapCore::kMediumCountryLabelZoom;
  }
  return bitslate::geo::ZoomableWorldMapCore::kSmallCountryLabelZoom;
}

void positionOverlayLabel(lv_obj_t* mainLabel, int x, int y, bool visible) {
  hideObj(mainLabel, !visible);
  if (!visible || mainLabel == nullptr) {
    return;
  }
  lv_obj_set_pos(mainLabel, x, y);
  lv_obj_move_foreground(mainLabel);
}

void redrawMap() {
  if (mapPixels == nullptr) {
    return;
  }
  mapCore.renderMapRgb565(mapPixels, MAP_W, MAP_H, renderOptions);
  if (mapImage != nullptr) {
    lv_obj_invalidate(mapImage);
  }
}

void positionOverlays() {
  const bitslate::geo::WorldCapitalMarker* capitals = mapCore.capitalMarkers();
  for (uint8_t i = 0; i < mapCore.capitalMarkerCount(); ++i) {
    float sx = 0.0f;
    float sy = 0.0f;
    mapCore.gridToScreen(capitals[i].gridX, capitals[i].gridY, MAP_W, MAP_H, sx, sy);
    const bool visible = screenPointVisible(sx, sy, 12);

    hideObj(capitalDots[i], !visible);
    if (visible && capitalDots[i] != nullptr) {
      lv_obj_set_pos(
          capitalDots[i],
          static_cast<int>(sx) - CAPITAL_DOT_SIZE / 2,
          static_cast<int>(sy) - CAPITAL_DOT_SIZE / 2
      );
      lv_obj_move_foreground(capitalDots[i]);
    }

    const bool showCapitalLabel =
        labelsEnabled && mapCore.zoom() >= bitslate::geo::ZoomableWorldMapCore::kCapitalLabelZoom && visible;
    positionOverlayLabel(
        capitalLabels[i],
        static_cast<int>(sx) - CAPITAL_LABEL_W / 2,
        static_cast<int>(sy) - CAPITAL_DOT_SIZE / 2 - CAPITAL_LABEL_H - 3,
        showCapitalLabel
    );
  }

  const bitslate::geo::WorldLabelPoint* countries = mapCore.countryLabelPoints();
  for (uint8_t i = 0; i < mapCore.countryLabelPointCount(); ++i) {
    float sx = 0.0f;
    float sy = 0.0f;
    mapCore.gridToScreen(countries[i].gridX, countries[i].gridY, MAP_W, MAP_H, sx, sy);
    const float threshold = countryLabelZoomThreshold(countries[i].cellCount);
    const bool showCountryLabel =
        labelsEnabled && mapCore.zoom() >= threshold &&
        screenPointVisible(sx, sy, 24);
    positionOverlayLabel(
        countryLabels[i],
        static_cast<int>(sx) - COUNTRY_LABEL_W / 2,
        static_cast<int>(sy) - COUNTRY_LABEL_H / 2,
        showCountryLabel
    );
  }

  const bitslate::geo::WorldOceanLabel* oceans = mapCore.oceanLabels();
  for (uint8_t i = 0; i < mapCore.oceanLabelCount(); ++i) {
    float sx = 0.0f;
    float sy = 0.0f;
    mapCore.gridToScreen(oceans[i].gridX, oceans[i].gridY, MAP_W, MAP_H, sx, sy);
    const bool showOceanLabel =
        labelsEnabled && mapCore.zoom() >= bitslate::geo::ZoomableWorldMapCore::kOceanLabelZoom &&
        screenPointVisible(sx, sy, 36);
    positionOverlayLabel(
        oceanLabels[i],
        static_cast<int>(sx) - OCEAN_LABEL_W / 2,
        static_cast<int>(sy) - OCEAN_LABEL_H / 2,
        showOceanLabel
    );
  }
}

void refreshMapView() {
  redrawMap();
  positionOverlays();
}

void refreshGridButton() {
  if (gridButtonLabel != nullptr) {
    lv_label_set_text(gridButtonLabel, renderOptions.showBitGrid ? "Grid" : "No Grid");
    lv_obj_center(gridButtonLabel);
  }
}

void refreshLabelsButton() {
  if (labelsButtonLabel != nullptr) {
    lv_label_set_text(labelsButtonLabel, labelsEnabled ? "Labels" : "No Label");
    lv_obj_center(labelsButtonLabel);
  }
}

void zoomInEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) {
    return;
  }
  mapCore.zoomAtScreenPoint(MAP_W / 2.0f, MAP_H / 2.0f, 1.35f, MAP_W, MAP_H);
  refreshMapView();
}

void zoomOutEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) {
    return;
  }
  mapCore.zoomAtScreenPoint(MAP_W / 2.0f, MAP_H / 2.0f, 1.0f / 1.35f, MAP_W, MAP_H);
  refreshMapView();
}

void resetEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) {
    return;
  }
  mapCore.resetCamera();
  setStatus("World Map");
  refreshMapView();
}

void gridEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) {
    return;
  }
  renderOptions.showBitGrid = !renderOptions.showBitGrid;
  refreshGridButton();
  refreshMapView();
}

void labelsEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) {
    return;
  }
  labelsEnabled = !labelsEnabled;
  refreshLabelsButton();
  positionOverlays();
}

bool pointInMap(const lv_point_t& point, int& localX, int& localY) {
  lv_area_t coords;
  lv_obj_get_coords(mapArea, &coords);
  localX = point.x - coords.x1;
  localY = point.y - coords.y1;
  return localX >= 0 && localY >= 0 && localX < MAP_W && localY < MAP_H;
}

void handleMapTap(const lv_point_t& point) {
  int localX = 0;
  int localY = 0;
  if (!pointInMap(point, localX, localY)) {
    setStatus("Ocean");
    return;
  }

  const uint8_t countryId = mapCore.countryAtScreen(
      static_cast<float>(localX),
      static_cast<float>(localY),
      MAP_W,
      MAP_H
  );
  const char* name = countryName(countryId);
  if (name == nullptr) {
    setStatus("Ocean");
    return;
  }

  char status[64];
  std::snprintf(status, sizeof(status), "Clicked: %s", name);
  setStatus(status);
}

void mapPointerEvent(lv_event_t* event) {
  const lv_event_code_t code = lv_event_get_code(event);
  lv_indev_t* indev = lv_event_get_indev(event);
  if (indev == nullptr) {
    return;
  }

  lv_point_t point;
  lv_indev_get_point(indev, &point);

  if (code == LV_EVENT_PRESSED) {
    pointerDown = true;
    dragging = false;
    dragDistance = 0;
    pressPoint = point;
    lastPoint = point;
    return;
  }

  if (code == LV_EVENT_PRESSING && pointerDown) {
    const int dx = point.x - lastPoint.x;
    const int dy = point.y - lastPoint.y;
    dragDistance += LV_ABS(dx) + LV_ABS(dy);
    lastPoint = point;

    if (dragDistance > DRAG_TAP_THRESHOLD) {
      dragging = true;
      mapCore.panByPixels(static_cast<float>(dx), static_cast<float>(dy), MAP_W, MAP_H);
      refreshMapView();
    }
    return;
  }

  if (code == LV_EVENT_RELEASED && pointerDown) {
    pointerDown = false;
    if (!dragging && dragDistance <= DRAG_TAP_THRESHOLD) {
      handleMapTap(point);
    }
  }
}

lv_obj_t* makeButton(lv_obj_t* parent, const char* text, int x, int y, int w, lv_event_cb_t cb) {
  lv_obj_t* button = lv_obj_create(parent);
  styleBox(button, 0x334155, 0x111827, 2, 4);
  lv_obj_set_pos(button, x, y);
  lv_obj_set_size(button, w, BUTTON_H);
  lv_obj_set_style_bg_color(button, color(0x475569), LV_STATE_PRESSED);
  lv_obj_set_style_translate_y(button, 1, LV_STATE_PRESSED);
  lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(button, cb, LV_EVENT_CLICKED, nullptr);

  lv_obj_t* label = makeLabel(button, text, &monogram_20, 0xF8FAFC);
  lv_obj_center(label);
  return label;
}

void createOverlays() {
  const bitslate::geo::WorldCapitalMarker* capitals = mapCore.capitalMarkers();
  for (uint8_t i = 0; i < mapCore.capitalMarkerCount(); ++i) {
    capitalDots[i] = lv_obj_create(mapArea);
    styleBox(capitalDots[i], 0x101418, 0xF8FAFC, 1, CAPITAL_DOT_SIZE / 2);
    lv_obj_set_size(capitalDots[i], CAPITAL_DOT_SIZE, CAPITAL_DOT_SIZE);

    capitalLabels[i] = makeLabel(mapArea, capitals[i].capitalName, &monogram_16, 0xFFFFFF);
    lv_obj_set_size(capitalLabels[i], CAPITAL_LABEL_W, CAPITAL_LABEL_H);
    styleOverlayLabel(capitalLabels[i], 0xFFFFFF);
  }

  const bitslate::geo::WorldLabelPoint* countries = mapCore.countryLabelPoints();
  for (uint8_t i = 0; i < mapCore.countryLabelPointCount(); ++i) {
    countryLabels[i] = makeLabel(mapArea, countries[i].countryName, &monogram_16, 0xFFFFFF);
    lv_obj_set_size(countryLabels[i], COUNTRY_LABEL_W, COUNTRY_LABEL_H);
    styleOverlayLabel(countryLabels[i], 0xFFFFFF);
  }

  const bitslate::geo::WorldOceanLabel* oceans = mapCore.oceanLabels();
  for (uint8_t i = 0; i < mapCore.oceanLabelCount(); ++i) {
    oceanLabels[i] = makeLabel(mapArea, oceans[i].name, &monogram_16, 0xEAF6FF);
    lv_obj_set_size(oceanLabels[i], OCEAN_LABEL_W, OCEAN_LABEL_H);
    styleOverlayLabel(oceanLabels[i], 0xEAF6FF);
  }
}

void createMap(lv_obj_t* screen) {
  mapArea = lv_obj_create(screen);
  styleBox(mapArea, 0x27346D, 0xCBD5E1, 1, 2);
  lv_obj_set_pos(mapArea, MAP_X, MAP_Y);
  lv_obj_set_size(mapArea, MAP_W, MAP_H);
  lv_obj_add_flag(mapArea, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(mapArea, mapPointerEvent, LV_EVENT_PRESSED, nullptr);
  lv_obj_add_event_cb(mapArea, mapPointerEvent, LV_EVENT_PRESSING, nullptr);
  lv_obj_add_event_cb(mapArea, mapPointerEvent, LV_EVENT_RELEASED, nullptr);

  if (mapPixels == nullptr) {
    mapPixels = static_cast<uint16_t*>(heap_caps_malloc(MAP_BYTES, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (mapPixels != nullptr) {
      bufferStatus = "Map buffer: PSRAM";
    }
  }
  if (mapPixels == nullptr) {
    mapPixels = static_cast<uint16_t*>(heap_caps_malloc(MAP_BYTES, MALLOC_CAP_8BIT));
    if (mapPixels != nullptr) {
      bufferStatus = "Map buffer: internal";
    }
  }
  if (mapPixels == nullptr) {
    bufferStatus = "Map buffer failed";
    lv_obj_t* failureLabel = makeLabel(mapArea, "Map buffer failed", &lv_font_montserrat_14, 0x991B1B);
    lv_obj_center(failureLabel);
    setStatus("Map buffer failed");
    Serial.printf("Zoomable world map buffer allocation failed (%d bytes)\n", MAP_BYTES);
    return;
  }

  Serial.printf("Zoomable world map %s (%d bytes)\n", bufferStatus, MAP_BYTES);

  mapImageDsc.header.magic = LV_IMAGE_HEADER_MAGIC;
  mapImageDsc.header.cf = LV_COLOR_FORMAT_RGB565;
  mapImageDsc.header.flags = 0;
  mapImageDsc.header.w = MAP_W;
  mapImageDsc.header.h = MAP_H;
  mapImageDsc.header.stride = MAP_W * 2;
  mapImageDsc.data_size = MAP_BYTES;
  mapImageDsc.data = reinterpret_cast<const uint8_t*>(mapPixels);

  mapImage = lv_image_create(mapArea);
  lv_image_set_src(mapImage, &mapImageDsc);
  lv_obj_set_pos(mapImage, 0, 0);
  lv_obj_set_size(mapImage, MAP_W, MAP_H);
  clearInteraction(mapImage);

  createOverlays();
  positionOverlays();
}

}  // namespace

void ZoomableWorldMapApp::create() {
  mapCore.resetCamera();
  renderOptions.showBitGrid = true;
  renderOptions.showBorders = true;
  labelsEnabled = true;
  pointerDown = false;
  dragging = false;
  dragDistance = 0;

  lv_obj_t* screen = lv_obj_create(nullptr);
  lv_scr_load(screen);
  styleBox(screen, 0xEEF2F7, 0xEEF2F7, 0, 0);
  lv_obj_set_size(screen, SCREEN_W, SCREEN_H);

  createMap(screen);

  int x = 45;
  makeButton(screen, "+", x, BUTTON_Y, SMALL_BUTTON_W, zoomInEvent);
  x += SMALL_BUTTON_W + 8;
  makeButton(screen, "-", x, BUTTON_Y, SMALL_BUTTON_W, zoomOutEvent);
  x += SMALL_BUTTON_W + 8;
  makeButton(screen, "Reset", x, BUTTON_Y, RESET_BUTTON_W, resetEvent);
  x += RESET_BUTTON_W + 8;
  gridButtonLabel = makeButton(screen, "Grid", x, BUTTON_Y, GRID_BUTTON_W, gridEvent);
  x += GRID_BUTTON_W + 8;
  labelsButtonLabel = makeButton(screen, "Labels", x, BUTTON_Y, LABELS_BUTTON_W, labelsEvent);
  refreshGridButton();
  refreshLabelsButton();

  refreshMapView();
}

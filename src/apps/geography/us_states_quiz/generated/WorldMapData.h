#pragma once

#include <stdint.h>

namespace bitslate::geo {

static constexpr uint16_t WORLD_GRID_W = 360;
static constexpr uint16_t WORLD_GRID_H = 180;
static constexpr uint8_t WORLD_COUNTRY_COUNT = 176;
static constexpr uint8_t WORLD_COUNTRY_NONE = 255;
static constexpr uint8_t WORLD_CAPITAL_MARKER_COUNT = 41;
static constexpr uint8_t WORLD_COUNTRY_LABEL_COUNT = 41;
static constexpr uint8_t WORLD_OCEAN_LABEL_COUNT = 5;

struct WorldCountryInfo {
  const char* name;
  uint8_t id;
};

struct WorldMapCell {
  uint16_t x;
  uint16_t y;
};

struct WorldCapitalMarker {
  uint8_t countryId;
  const char* countryName;
  const char* capitalName;
  float gridX;
  float gridY;
};

struct WorldLabelPoint {
  uint8_t countryId;
  const char* countryName;
  float gridX;
  float gridY;
  uint16_t cellCount;
};

struct WorldOceanLabel {
  const char* name;
  float gridX;
  float gridY;
};

extern const WorldCountryInfo WORLD_COUNTRIES[WORLD_COUNTRY_COUNT];
extern const uint8_t WORLD_COUNTRY_ID_GRID[WORLD_GRID_W * WORLD_GRID_H];
extern const WorldMapCell WORLD_BORDER_CELLS[];
extern const uint16_t WORLD_BORDER_CELL_COUNT;
extern const WorldCapitalMarker WORLD_CAPITAL_MARKERS[WORLD_CAPITAL_MARKER_COUNT];
extern const WorldLabelPoint WORLD_COUNTRY_LABEL_POINTS[WORLD_COUNTRY_LABEL_COUNT];
extern const WorldOceanLabel WORLD_OCEAN_LABELS[WORLD_OCEAN_LABEL_COUNT];

int findWorldCountryByName(const char* name);
int worldCountryAtCell(uint16_t x, uint16_t y);

}  // namespace bitslate::geo

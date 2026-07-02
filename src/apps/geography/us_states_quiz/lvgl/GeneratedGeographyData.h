#pragma once

#include <stdint.h>

struct GeoCell {
  uint8_t x;
  uint8_t y;
};

struct GeoStateMask {
  const char* name;
  const GeoCell* cells;
  uint16_t count;
};

extern const uint16_t GEO_GRID_WIDTH;
extern const uint16_t GEO_GRID_HEIGHT;
extern const GeoCell GEO_MAP_CELLS[];
extern const uint16_t GEO_MAP_CELL_COUNT;
extern const GeoStateMask GEO_STATE_MASKS[];
extern const uint8_t GEO_STATE_COUNT;

int findGeoStateAtCell(uint8_t x, uint8_t y);

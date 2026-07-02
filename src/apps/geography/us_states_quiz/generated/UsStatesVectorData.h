#pragma once

#include <stdint.h>

namespace bitslate::geo {

static constexpr uint16_t US_GRID_W = 240;
static constexpr uint16_t US_GRID_H = 150;
static constexpr uint8_t US_STATE_COUNT = 48;
static constexpr uint8_t US_STATE_NONE = 255;

struct UsStateInfo {
  const char* name;
  const char* abbrev;
  uint8_t id;
};

struct GeoCell {
  uint16_t x;
  uint16_t y;
};

extern const UsStateInfo US_STATES[US_STATE_COUNT];
extern const uint8_t US_STATE_ID_GRID[US_GRID_W * US_GRID_H];
extern const GeoCell US_BORDER_CELLS[];
extern const uint16_t US_BORDER_CELL_COUNT;

int findStateByName(const char* name);
int stateAtCell(uint16_t x, uint16_t y);

}  // namespace bitslate::geo

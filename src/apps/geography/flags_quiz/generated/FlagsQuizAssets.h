#pragma once

#include <stdint.h>
#include <lvgl.h>

namespace bitslate::geo::flags {

struct FlagQuizAsset {
  const char* name;
  const char* slug;
  const lv_image_dsc_t* image;
  uint16_t width;
  uint16_t height;
};

extern const FlagQuizAsset FLAGS_QUIZ_ASSETS[];
extern const uint8_t FLAGS_QUIZ_ASSET_COUNT;

}  // namespace bitslate::geo::flags

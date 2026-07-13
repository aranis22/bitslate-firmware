#pragma once

#ifdef __cplusplus
class ZoomableWorldMapApp {
 public:
  static void create();
};

extern "C" {
#endif

void zoomable_world_map_app_create(void);

#ifdef __cplusplus
}
#endif

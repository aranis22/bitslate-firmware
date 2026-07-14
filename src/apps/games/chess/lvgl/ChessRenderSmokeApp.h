#pragma once

#ifdef __cplusplus
class ChessRenderSmokeApp {
public:
  static void create();
};

extern "C" {
#endif

void chess_render_smoke_app_create(void);
void chess_render_smoke_app_destroy(void);

#ifdef __cplusplus
}
#endif

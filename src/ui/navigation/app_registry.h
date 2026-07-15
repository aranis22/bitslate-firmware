#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  APP_ID_NONE = 0,
  APP_ID_CHESS,
  APP_ID_WORLD_MAP,
  APP_ID_MASSES_SPRINGS,
  APP_ID_COLLISION,
  APP_ID_CIRCUITS,
  APP_ID_PERIODIC_TABLE,
  APP_ID_ATOM_LAB,
  APP_ID_GRAPHING_CALCULATOR,
  APP_ID_SCIENTIFIC_CALCULATOR,
  APP_ID_US_STATES_QUIZ,
  APP_ID_WORLD_FLAGS_QUIZ,
  APP_ID_BITBLOCKS,
} app_id_t;

typedef void (*app_callback_t)(void);

typedef struct {
  app_id_t id;
  const char *display_name;
  app_callback_t start;
  app_callback_t cleanup;
} app_descriptor_t;

const app_descriptor_t *app_registry_find(app_id_t id);

#ifdef __cplusplus
}
#endif

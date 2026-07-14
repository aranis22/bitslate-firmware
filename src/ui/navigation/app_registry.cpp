#include "app_registry.h"

#include "apps/games/chess/lvgl/ChessRenderSmokeApp.h"
#include "apps/geography/flags_quiz/lvgl/FlagsQuizApp.h"
#include "apps/geography/us_states_quiz/lvgl/UsStatesQuizApp.h"
#include "apps/geography/us_states_quiz/lvgl/ZoomableWorldMapApp.h"
#include "apps/stem/chemistry/build_atom/BuildAtomApp.h"
#include "apps/stem/chemistry/periodic_table/PeriodicTableApp.h"
#include "apps/stem/math/calculator/ScientificCalculatorApp.h"
#include "apps/stem/math/graphing_calculator/GraphingCalculatorApp.h"
#include "apps/stem/physics/circuit_lab/CircuitLabApp.h"
#include "apps/stem/physics/collision_lab/CollisionLabApp.h"
#include "apps/stem/physics/masses_springs/MassesSpringsApp.h"

namespace {
void start_chess() { chess_render_smoke_app_create(); }
void stop_chess() { chess_render_smoke_app_destroy(); }
void start_world_map() { zoomable_world_map_app_create(); }
void stop_world_map() { zoomable_world_map_app_destroy(); }

const app_descriptor_t APPS[] = {
    {APP_ID_CHESS, "Chess", start_chess, stop_chess},
    {APP_ID_WORLD_MAP, "World Map", start_world_map, stop_world_map},
    {APP_ID_MASSES_SPRINGS, "Masses / Springs", MassesSpringsApp::create, MassesSpringsApp::destroy},
    {APP_ID_COLLISION, "Collision", CollisionLabApp::create, CollisionLabApp::destroy},
    {APP_ID_CIRCUITS, "Circuits", CircuitLabApp::create, nullptr},
    {APP_ID_PERIODIC_TABLE, "Periodic Table", PeriodicTableApp::create, nullptr},
    {APP_ID_ATOM_LAB, "Atom Lab", BuildAtomApp::create, nullptr},
    {APP_ID_GRAPHING_CALCULATOR, "Graphing Calculator", GraphingCalculatorApp::create, nullptr},
    {APP_ID_SCIENTIFIC_CALCULATOR, "Scientific Calculator", ScientificCalculatorApp::create, nullptr},
    {APP_ID_US_STATES_QUIZ, "US States Quiz", UsStatesQuizApp::create, UsStatesQuizApp::destroy},
    {APP_ID_WORLD_FLAGS_QUIZ, "World Flags Quiz", FlagsQuizApp::create, FlagsQuizApp::destroy},
};
}

extern "C" const app_descriptor_t *app_registry_find(app_id_t id) {
  for(const app_descriptor_t &app : APPS) if(app.id == id) return &app;
  return nullptr;
}

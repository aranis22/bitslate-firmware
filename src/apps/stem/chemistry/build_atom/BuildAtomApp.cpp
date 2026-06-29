#include "BuildAtomApp.h"

#include <cmath>
#include <cstdio>

#include <lvgl.h>

#include "AtomModel.h"

namespace {

constexpr int SCREEN_W = 480;
constexpr int SCREEN_H = 320;
constexpr int TRAY_W = 104;
constexpr int ATOM_CX = 244;
constexpr int ATOM_CY = 164;
constexpr int NUCLEUS_R = 38;
constexpr int SHELL_1_R = 58;
constexpr int SHELL_2_R = 90;
constexpr int CARD_X = 358;
constexpr int CARD_Y = 54;
constexpr int CARD_W = 112;
constexpr int CARD_H = 176;
constexpr int MAX_NUCLEUS_PARTICLES = 26;
constexpr int MAX_ELECTRONS = 10;

AtomModel model;
lv_obj_t* symbolLabel = nullptr;
lv_obj_t* elementLabel = nullptr;
lv_obj_t* atomicLabel = nullptr;
lv_obj_t* massLabel = nullptr;
lv_obj_t* chargeLabel = nullptr;
lv_obj_t* neutralLabel = nullptr;
lv_obj_t* protonCountLabel = nullptr;
lv_obj_t* neutronCountLabel = nullptr;
lv_obj_t* electronCountLabel = nullptr;
lv_obj_t* nucleusParticles[MAX_NUCLEUS_PARTICLES] = {};
lv_obj_t* electrons[MAX_ELECTRONS] = {};

lv_color_t color(uint32_t hex) {
  return lv_color_hex(hex);
}

void clearInteraction(lv_obj_t* obj) {
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

void styleBox(lv_obj_t* obj, uint32_t bg, uint32_t border, int borderWidth, int radius = 0) {
  lv_obj_remove_style_all(obj);
  lv_obj_set_style_bg_color(obj, color(bg), 0);
  lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(obj, color(border), 0);
  lv_obj_set_style_border_width(obj, borderWidth, 0);
  lv_obj_set_style_radius(obj, radius, 0);
  clearInteraction(obj);
}

lv_obj_t* makeLabel(lv_obj_t* parent, const char* text, int x, int y, const lv_font_t* font = &lv_font_montserrat_14) {
  lv_obj_t* label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_obj_set_pos(label, x, y);
  lv_obj_set_style_text_color(label, color(0x1C2638), 0);
  lv_obj_set_style_text_font(label, font, 0);
  clearInteraction(label);
  return label;
}

lv_obj_t* makeCircle(lv_obj_t* parent, int cx, int cy, int radius, uint32_t fill, uint32_t border, int borderWidth) {
  lv_obj_t* obj = lv_obj_create(parent);
  styleBox(obj, fill, border, borderWidth, LV_RADIUS_CIRCLE);
  lv_obj_set_size(obj, radius * 2, radius * 2);
  lv_obj_set_pos(obj, cx - radius, cy - radius);
  return obj;
}

void setObjVisible(lv_obj_t* obj, bool visible) {
  if (obj == nullptr) return;
  if (visible) {
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
  }
}

void updateParticleObj(lv_obj_t* obj, int cx, int cy, uint32_t fill) {
  styleBox(obj, fill, 0xFFFFFF, 1, LV_RADIUS_CIRCLE);
  lv_obj_set_size(obj, 16, 16);
  lv_obj_set_pos(obj, cx - 8, cy - 8);
  setObjVisible(obj, true);
}

void updateNucleusParticles() {
  static constexpr int offsets[][2] = {
      {0, 0}, {-13, -8}, {13, -8}, {-13, 10}, {13, 10}, {0, -22}, {0, 22},
      {-25, 0}, {25, 0}, {-23, -19}, {23, 19}, {23, -19}, {-23, 19},
      {0, -34}, {0, 34}, {-35, 0}, {35, 0}, {-33, -28}, {33, -28},
      {-33, 28}, {33, 28}, {-10, -32}, {10, 32}, {10, -32}, {-10, 32},
      {0, 12},
  };

  int index = 0;
  for (int i = 0; i < model.getProtons() && index < MAX_NUCLEUS_PARTICLES; ++i, ++index) {
    updateParticleObj(nucleusParticles[index], ATOM_CX + offsets[index][0], ATOM_CY + offsets[index][1], 0xE04A4A);
  }
  for (int i = 0; i < model.getNeutrons() && index < MAX_NUCLEUS_PARTICLES; ++i, ++index) {
    updateParticleObj(nucleusParticles[index], ATOM_CX + offsets[index][0], ATOM_CY + offsets[index][1], 0x767E88);
  }
  for (; index < MAX_NUCLEUS_PARTICLES; ++index) {
    setObjVisible(nucleusParticles[index], false);
  }
}

void updateElectrons() {
  const int count = model.getElectrons();
  for (int i = 0; i < MAX_ELECTRONS; ++i) {
    if (i >= count) {
      setObjVisible(electrons[i], false);
      continue;
    }

    const bool shellOne = i < 2;
    const int shellIndex = shellOne ? i : i - 2;
    const int shellCount = shellOne ? (count < 2 ? count : 2) : (count - 2);
    const int radius = shellOne ? SHELL_1_R : SHELL_2_R;
    const float angle = (6.2831853f * shellIndex / shellCount) - 1.5707963f;
    const int x = ATOM_CX + static_cast<int>(std::cos(angle) * radius);
    const int y = ATOM_CY + static_cast<int>(std::sin(angle) * radius);
    updateParticleObj(electrons[i], x, y, 0x3D74E2);
  }
}

void updateReadouts() {
  char buf[64];

  lv_label_set_text(symbolLabel, model.getElementSymbol());
  std::snprintf(buf, sizeof(buf), "Element\n%s", model.getElementName());
  lv_label_set_text(elementLabel, buf);
  std::snprintf(buf, sizeof(buf), "Atomic No: %d", model.getAtomicNumber());
  lv_label_set_text(atomicLabel, buf);
  std::snprintf(buf, sizeof(buf), "Mass No: %d", model.getMassNumber());
  lv_label_set_text(massLabel, buf);
  std::snprintf(buf, sizeof(buf), "Charge: %d", model.getCharge());
  lv_label_set_text(chargeLabel, buf);
  lv_label_set_text(neutralLabel, model.isNeutral() ? "Neutral" : "Ion");

  std::snprintf(buf, sizeof(buf), "%d", model.getProtons());
  lv_label_set_text(protonCountLabel, buf);
  std::snprintf(buf, sizeof(buf), "%d", model.getNeutrons());
  lv_label_set_text(neutronCountLabel, buf);
  std::snprintf(buf, sizeof(buf), "%d", model.getElectrons());
  lv_label_set_text(electronCountLabel, buf);
}

void render() {
  updateNucleusParticles();
  updateElectrons();
  updateReadouts();
}

void buttonEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;

  const char* action = static_cast<const char*>(lv_event_get_user_data(event));
  if (action == nullptr) return;

  switch (action[0]) {
    case 'p':
      model.addProton();
      break;
    case 'n':
      model.addNeutron();
      break;
    case 'e':
      model.addElectron();
      break;
    case 'r':
      model.reset();
      break;
    default:
      break;
  }
  render();
}

lv_obj_t* makeTrayButton(lv_obj_t* parent, const char* text, const char* action, int y, uint32_t particleColor, lv_obj_t** countLabel) {
  lv_obj_t* btn = lv_button_create(parent);
  lv_obj_set_pos(btn, 10, y);
  lv_obj_set_size(btn, 84, 58);
  lv_obj_set_style_bg_color(btn, color(0xEEF2F8), 0);
  lv_obj_set_style_border_color(btn, color(0xA8B5C8), 0);
  lv_obj_set_style_border_width(btn, 1, 0);
  lv_obj_set_style_radius(btn, 6, 0);
  lv_obj_add_event_cb(btn, buttonEvent, LV_EVENT_CLICKED, const_cast<char*>(action));

  lv_obj_t* label = lv_label_create(btn);
  lv_label_set_text(label, text);
  lv_obj_set_style_text_color(label, color(0x1C2638), 0);
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 5);

  lv_obj_t* dot = lv_obj_create(btn);
  styleBox(dot, particleColor, 0xFFFFFF, 1, LV_RADIUS_CIRCLE);
  lv_obj_set_size(dot, 15, 15);
  lv_obj_align(dot, LV_ALIGN_BOTTOM_MID, 0, -2);

  *countLabel = lv_label_create(btn);
  lv_label_set_text(*countLabel, "0");
  lv_obj_set_style_text_color(*countLabel, color(0x566274), 0);
  lv_obj_align(*countLabel, LV_ALIGN_BOTTOM_RIGHT, -3, -1);
  return btn;
}

lv_obj_t* makeResetButton(lv_obj_t* parent) {
  lv_obj_t* btn = lv_button_create(parent);
  lv_obj_set_pos(btn, 10, 250);
  lv_obj_set_size(btn, 84, 44);
  lv_obj_set_style_bg_color(btn, color(0xEEF2F8), 0);
  lv_obj_set_style_border_color(btn, color(0xA8B5C8), 0);
  lv_obj_set_style_border_width(btn, 1, 0);
  lv_obj_set_style_radius(btn, 6, 0);
  lv_obj_add_event_cb(btn, buttonEvent, LV_EVENT_CLICKED, const_cast<char*>("r"));

  lv_obj_t* label = lv_label_create(btn);
  lv_label_set_text(label, "Reset");
  lv_obj_set_style_text_color(label, color(0x1C2638), 0);
  lv_obj_center(label);
  return btn;
}

} // namespace

void BuildAtomApp::create() {
  model.reset();

  lv_obj_t* screen = lv_obj_create(nullptr);
  lv_scr_load(screen);
  styleBox(screen, 0xDEEFFF, 0xDEEFFF, 0);
  lv_obj_set_size(screen, SCREEN_W, SCREEN_H);

  lv_obj_t* tray = lv_obj_create(screen);
  styleBox(tray, 0xF7F9FC, 0x9EAFC6, 1);
  lv_obj_set_pos(tray, 0, 0);
  lv_obj_set_size(tray, TRAY_W, SCREEN_H);

  makeTrayButton(tray, "Proton", "p", 24, 0xE04A4A, &protonCountLabel);
  makeTrayButton(tray, "Neutron", "n", 92, 0x767E88, &neutronCountLabel);
  makeTrayButton(tray, "Electron", "e", 160, 0x3D74E2, &electronCountLabel);
  makeResetButton(tray);

  makeLabel(screen, "Build an Atom", 124, 16, &lv_font_montserrat_22);

  lv_obj_t* card = lv_obj_create(screen);
  styleBox(card, 0xF8FBFF, 0x8EA2B8, 2, 3);
  lv_obj_set_pos(card, CARD_X, CARD_Y);
  lv_obj_set_size(card, CARD_W, CARD_H);

  makeCircle(screen, ATOM_CX, ATOM_CY, SHELL_2_R, 0xDEEFFF, 0x9AB2E5, 2);
  makeCircle(screen, ATOM_CX, ATOM_CY, SHELL_1_R, 0xDEEFFF, 0xB0C3EE, 2);
  makeLabel(screen, "shell 2", ATOM_CX - 26, ATOM_CY - SHELL_2_R - 22);
  makeLabel(screen, "shell 1", ATOM_CX - 26, ATOM_CY - SHELL_1_R - 20);
  makeCircle(screen, ATOM_CX, ATOM_CY, NUCLEUS_R, 0xECC471, 0x765638, 2);

  for (int i = 0; i < MAX_NUCLEUS_PARTICLES; ++i) {
    nucleusParticles[i] = makeCircle(screen, ATOM_CX, ATOM_CY, 8, 0xE04A4A, 0xFFFFFF, 1);
  }
  for (int i = 0; i < MAX_ELECTRONS; ++i) {
    electrons[i] = makeCircle(screen, ATOM_CX, ATOM_CY, 7, 0x3D74E2, 0xFFFFFF, 1);
  }

  symbolLabel = makeLabel(card, "?", 0, 10, &lv_font_montserrat_28);
  lv_obj_set_width(symbolLabel, CARD_W);
  lv_obj_set_style_text_align(symbolLabel, LV_TEXT_ALIGN_CENTER, 0);
  elementLabel = makeLabel(card, "Element\nUnknown", 8, 45);
  atomicLabel = makeLabel(card, "Atomic No: 0", 8, 86);
  massLabel = makeLabel(card, "Mass No: 0", 8, 112);
  chargeLabel = makeLabel(card, "Charge: 0", 8, 134);
  neutralLabel = makeLabel(card, "Neutral", 8, 154);
  makeLabel(screen, "Tap tray buttons to add particles.", 122, 288);

  render();
}

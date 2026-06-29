#include "PeriodicTableApp.h"

#include <Arduino.h>
#include <cstdint>
#include <cstring>
#include <cstdio>

#include <lvgl.h>

#include "PeriodicTableModel.h"

namespace {

constexpr int SCREEN_W = 480;
constexpr int SCREEN_H = 320;
constexpr int PANEL_W = 125;
constexpr int GRID_X = 130;
constexpr int GRID_Y = 40;
constexpr int CELL_W = 18;
constexpr int CELL_H = 25;
constexpr int CELL_GAP = 1;
constexpr int MAX_ELEMENTS = 118;

PeriodicTableModel model;
lv_obj_t* symbolLabel = nullptr;
lv_obj_t* nameLabel = nullptr;
lv_obj_t* atomicLabel = nullptr;
lv_obj_t* massLabel = nullptr;
lv_obj_t* categoryLabel = nullptr;
lv_obj_t* groupPeriodLabel = nullptr;
lv_obj_t* shellsLabel = nullptr;
lv_obj_t* stateLabel = nullptr;
lv_obj_t* electronegativityLabel = nullptr;
lv_obj_t* cellLabels[MAX_ELEMENTS] = {};
bool stylesInitialized = false;
lv_style_t styleCellBase;
lv_style_t styleCellSelected;
lv_style_t styleAlkali;
lv_style_t styleAlkalineEarth;
lv_style_t styleTransition;
lv_style_t stylePostTransition;
lv_style_t styleMetalloid;
lv_style_t styleNonmetal;
lv_style_t styleHalogen;
lv_style_t styleNobleGas;
lv_style_t styleLanthanide;
lv_style_t styleActinide;
lv_style_t styleDefaultCategory;
constexpr int PANEL_TEXT_W = PANEL_W - 16;

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

lv_obj_t* makeLabel(lv_obj_t* parent, const char* text, int x, int y, const lv_font_t* font = nullptr) {
  lv_obj_t* label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_obj_set_pos(label, x, y);
  lv_obj_set_style_text_color(label, color(0x1C2638), 0);
  if (font != nullptr) {
    lv_obj_set_style_text_font(label, font, 0);
  }
  clearInteraction(label);
  return label;
}

lv_obj_t* makePanelDetailLabel(lv_obj_t* parent, const char* text, int x, int y) {
  lv_obj_t* label = makeLabel(parent, text, x, y, &lv_font_montserrat_14);
  lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(label, PANEL_TEXT_W);
  return label;
}

lv_obj_t* makePanelDetailLabelSized(lv_obj_t* parent, const char* text, int x, int y, int width) {
  lv_obj_t* label = makeLabel(parent, text, x, y, &lv_font_montserrat_14);
  lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(label, width);
  return label;
}

const char* safeText(const char* text) {
  return (text == nullptr || text[0] == '\0') ? "-" : text;
}

const char* compactCategoryText(const char* category) {
  if (category == nullptr || category[0] == '\0') return "-";
  if (std::strcmp(category, "Transition metal") == 0) return "Transition\nmetal";
  if (std::strcmp(category, "Post-transition metal") == 0) return "Post-transition\nmetal";
  if (std::strcmp(category, "Alkaline earth metal") == 0) return "Alkaline earth\nmetal";
  return category;
}

uint32_t categoryColor(const char* category) {
  if (category == nullptr) return 0xDCDCDC;
  if (std::strcmp(category, "Alkali metal") == 0) return 0xDEC963;
  if (std::strcmp(category, "Alkaline earth metal") == 0) return 0xE6EC69;
  if (std::strcmp(category, "Transition metal") == 0) return 0xE7BEB0;
  if (std::strcmp(category, "Post-transition metal") == 0) return 0xB1D7E1;
  if (std::strcmp(category, "Metalloid") == 0) return 0xA6DACD;
  if (std::strcmp(category, "Nonmetal") == 0) return 0xA5E291;
  if (std::strcmp(category, "Halogen") == 0) return 0xACEA91;
  if (std::strcmp(category, "Noble gas") == 0) return 0xD2B2D9;
  if (std::strcmp(category, "Lanthanide") == 0) return 0xE0CE97;
  if (std::strcmp(category, "Actinide") == 0) return 0xE1C1CF;
  return 0xDCDCDC;
}

void initStyle(lv_style_t& style, uint32_t bg) {
  lv_style_init(&style);
  lv_style_set_bg_opa(&style, LV_OPA_COVER);
  lv_style_set_bg_color(&style, color(bg));
  lv_style_set_border_color(&style, color(0x8D96A5));
  lv_style_set_border_width(&style, 1);
  lv_style_set_radius(&style, 1);
  lv_style_set_pad_all(&style, 0);
  lv_style_set_text_color(&style, color(0x132033));
  lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);
  lv_style_set_text_font(&style, &lv_font_montserrat_14);
}

void ensureStyles() {
  if (stylesInitialized) return;

  initStyle(styleCellBase, 0xDCDCDC);
  lv_style_set_bg_opa(&styleCellBase, LV_OPA_TRANSP);
  lv_style_set_border_opa(&styleCellBase, LV_OPA_TRANSP);

  lv_style_init(&styleCellSelected);
  lv_style_set_border_color(&styleCellSelected, color(0x184B9B));
  lv_style_set_border_width(&styleCellSelected, 2);

  initStyle(styleAlkali, 0xDEC963);
  initStyle(styleAlkalineEarth, 0xE6EC69);
  initStyle(styleTransition, 0xE7BEB0);
  initStyle(stylePostTransition, 0xB1D7E1);
  initStyle(styleMetalloid, 0xA6DACD);
  initStyle(styleNonmetal, 0xA5E291);
  initStyle(styleHalogen, 0xACEA91);
  initStyle(styleNobleGas, 0xD2B2D9);
  initStyle(styleLanthanide, 0xE0CE97);
  initStyle(styleActinide, 0xE1C1CF);
  initStyle(styleDefaultCategory, 0xDCDCDC);

  stylesInitialized = true;
}

lv_style_t* categoryStyle(const char* category) {
  if (category == nullptr) return &styleDefaultCategory;
  if (std::strcmp(category, "Alkali metal") == 0) return &styleAlkali;
  if (std::strcmp(category, "Alkaline earth metal") == 0) return &styleAlkalineEarth;
  if (std::strcmp(category, "Transition metal") == 0) return &styleTransition;
  if (std::strcmp(category, "Post-transition metal") == 0) return &stylePostTransition;
  if (std::strcmp(category, "Metalloid") == 0) return &styleMetalloid;
  if (std::strcmp(category, "Nonmetal") == 0) return &styleNonmetal;
  if (std::strcmp(category, "Halogen") == 0) return &styleHalogen;
  if (std::strcmp(category, "Noble gas") == 0) return &styleNobleGas;
  if (std::strcmp(category, "Lanthanide") == 0) return &styleLanthanide;
  if (std::strcmp(category, "Actinide") == 0) return &styleActinide;
  return &styleDefaultCategory;
}

void updateInfoPanel() {
  const PeriodicElement* element = model.getSelectedElement();
  if (element == nullptr) return;

  char buf[96];
  lv_label_set_text(symbolLabel, safeText(element->symbol));
  lv_label_set_text(nameLabel, safeText(element->name));
  std::snprintf(buf, sizeof(buf), "Atomic # %d", element->atomicNumber);
  lv_label_set_text(atomicLabel, buf);
  std::snprintf(buf, sizeof(buf), "Mass %s", safeText(element->atomicMass));
  lv_label_set_text(massLabel, buf);
  std::snprintf(buf, sizeof(buf), "Category\n%s", compactCategoryText(element->category));
  lv_label_set_text(categoryLabel, buf);
  std::snprintf(buf, sizeof(buf), "Group/Period %d / %d", element->group, element->period);
  lv_label_set_text(groupPeriodLabel, buf);
  std::snprintf(buf, sizeof(buf), "Shells %s", safeText(element->shells));
  lv_label_set_text(shellsLabel, buf);
  std::snprintf(buf, sizeof(buf), "State %s", safeText(element->state));
  lv_label_set_text(stateLabel, buf);
  std::snprintf(buf, sizeof(buf), "Electroneg. %s", safeText(element->electronegativity));
  lv_label_set_text(electronegativityLabel, buf);
}

void updateSelectionHighlight() {
  const PeriodicElement* selected = model.getSelectedElement();
  for (int i = 0; i < model.getElementCount(); ++i) {
    const PeriodicElement* element = &model.getAllElements()[i];
    if (cellLabels[i] == nullptr) continue;
    if (element == selected) {
      lv_obj_add_style(cellLabels[i], &styleCellSelected, 0);
    } else {
      lv_obj_remove_style(cellLabels[i], &styleCellSelected, 0);
    }
  }
}

void render() {
  updateInfoPanel();
  updateSelectionHighlight();
}

void cellEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  int atomicNumber = static_cast<int>(reinterpret_cast<intptr_t>(lv_event_get_user_data(event)));
  if (model.selectByAtomicNumber(atomicNumber)) {
    render();
  }
}

lv_obj_t* makeCell(lv_obj_t* parent, const PeriodicElement& element, int index) {
  lv_obj_t* label = lv_label_create(parent);
  lv_label_set_text(label, safeText(element.symbol));
  lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
  lv_obj_set_size(label, CELL_W, CELL_H);
  lv_obj_set_pos(
      label,
      GRID_X + (element.gridCol - 1) * (CELL_W + CELL_GAP),
      GRID_Y + (element.gridRow - 1) * (CELL_H + CELL_GAP));
  lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(label, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_style(label, &styleCellBase, 0);
  lv_obj_add_style(label, categoryStyle(element.category), 0);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_add_event_cb(label, cellEvent, LV_EVENT_CLICKED, reinterpret_cast<void*>(static_cast<intptr_t>(element.atomicNumber)));
  cellLabels[index] = label;
  return label;
}

}  // namespace

void PeriodicTableApp::create() {
  Serial.println("PeriodicTableApp: entered create");
  Serial.println("PeriodicTableApp: model initialized");
  ensureStyles();

  lv_obj_t* screen = lv_obj_create(nullptr);
  lv_scr_load(screen);
  styleBox(screen, 0xE8EEF6, 0xE8EEF6, 0);
  lv_obj_set_size(screen, SCREEN_W, SCREEN_H);
  Serial.println("PeriodicTableApp: screen/root created");

  lv_obj_t* panel = lv_obj_create(screen);
  styleBox(panel, 0xF5F8FC, 0xA1AEC0, 1);
  lv_obj_set_pos(panel, 0, 0);
  lv_obj_set_size(panel, PANEL_W, SCREEN_H);
  makeLabel(screen, "Periodic Table", GRID_X, 10, &lv_font_montserrat_22);

  symbolLabel = makeLabel(panel, "C", 8, 12, &lv_font_montserrat_28);
  nameLabel = makeLabel(panel, "Carbon", 8, 50, &lv_font_montserrat_18);
  lv_label_set_long_mode(nameLabel, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(nameLabel, PANEL_TEXT_W);
  atomicLabel = makePanelDetailLabel(panel, "Atomic # 6", 8, 88);
  massLabel = makePanelDetailLabel(panel, "Mass 12.011", 8, 108);
  categoryLabel = makePanelDetailLabel(panel, "Category\nNonmetal", 8, 130);
  groupPeriodLabel = makePanelDetailLabel(panel, "Group/Period 14 / 2", 8, 188);
  shellsLabel = makePanelDetailLabel(panel, "Shells 2, 4", 8, 218);
  stateLabel = makePanelDetailLabel(panel, "State Solid", 8, 248);
  electronegativityLabel = makePanelDetailLabel(panel, "Electroneg. 2.55", 8, 278);
  makeLabel(screen, "Tap an element", GRID_X, 292, &lv_font_montserrat_14);
  Serial.println("PeriodicTableApp: after left panel");

  Serial.println("PeriodicTableApp: before table cells");
  const PeriodicElement* elements = model.getAllElements();
  for (int i = 0; i < model.getElementCount(); ++i) {
    makeCell(screen, elements[i], i);
  }
  Serial.println("PeriodicTableApp: after cells created");

  render();
  Serial.println("PeriodicTableApp: complete");
}

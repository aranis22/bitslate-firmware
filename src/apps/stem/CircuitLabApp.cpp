#include "CircuitLabApp.h"

#include <lvgl.h>

namespace {

constexpr int SCREEN_W = 480;
constexpr int SCREEN_H = 320;
constexpr int SIDEBAR_W = 90;
constexpr int BOTTOM_TRAY_H = 42;
constexpr int CANVAS_X = SIDEBAR_W;
constexpr int CANVAS_W = SCREEN_W - SIDEBAR_W;
constexpr int CANVAS_H = SCREEN_H;

enum class Tool {
  Wire,
  Battery,
  Bulb,
  Resistor,
  Switch
};

Tool selectedTool = Tool::Wire;
lv_obj_t* canvasArea = nullptr;
lv_obj_t* wireLine = nullptr;
lv_obj_t* startNode = nullptr;
lv_obj_t* midNode = nullptr;
lv_obj_t* endNode = nullptr;
lv_obj_t* startTarget = nullptr;
lv_obj_t* endTarget = nullptr;
lv_obj_t* wireToolTile = nullptr;
lv_point_precise_t wirePoints[2] = {};
bool drawingWire = false;

lv_color_t color(uint32_t hex) {
  return lv_color_hex(hex);
}

void clearInteractionFlags(lv_obj_t* obj) {
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

void stylePanel(lv_obj_t* obj, uint32_t bg, uint32_t border = 0xC7C7C7, int borderWidth = 1) {
  lv_obj_remove_style_all(obj);
  lv_obj_set_style_bg_color(obj, color(bg), 0);
  lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(obj, color(border), 0);
  lv_obj_set_style_border_width(obj, borderWidth, 0);
  lv_obj_set_style_radius(obj, 0, 0);
  clearInteractionFlags(obj);
}

lv_obj_t* makeLabel(lv_obj_t* parent, const char* text, int y, const lv_font_t* font = &lv_font_montserrat_18) {
  lv_obj_t* label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_obj_set_style_text_color(label, color(0x000000), 0);
  lv_obj_set_style_text_font(label, font, 0);
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, y);
  return label;
}

lv_obj_t* makeCircle(lv_obj_t* parent, int x, int y, int size, uint32_t fill, uint32_t border, int borderWidth) {
  lv_obj_t* circle = lv_obj_create(parent);
  lv_obj_remove_style_all(circle);
  lv_obj_set_size(circle, size, size);
  lv_obj_set_pos(circle, x - size / 2, y - size / 2);
  lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(circle, color(fill), 0);
  lv_obj_set_style_bg_opa(circle, fill == 0xFFFFFF ? LV_OPA_TRANSP : LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(circle, color(border), 0);
  lv_obj_set_style_border_width(circle, borderWidth, 0);
  clearInteractionFlags(circle);
  return circle;
}

void setCircleCenter(lv_obj_t* obj, int x, int y) {
  int size = lv_obj_get_width(obj);
  lv_obj_set_pos(obj, x - size / 2, y - size / 2);
}

void createWireIcon(lv_obj_t* parent, int y) {
  lv_obj_t* line = lv_line_create(parent);
  static lv_point_precise_t iconPoints[] = {{18, 0}, {72, 0}};
  lv_line_set_points(line, iconPoints, 2);
  lv_obj_set_pos(line, 0, y);
  lv_obj_set_style_line_color(line, color(0x8E4F39), 0);
  lv_obj_set_style_line_width(line, 8, 0);
  lv_obj_set_style_line_rounded(line, true, 0);
}

void createBatteryIcon(lv_obj_t* parent, int y) {
  lv_obj_t* body = lv_obj_create(parent);
  lv_obj_remove_style_all(body);
  lv_obj_set_pos(body, 26, y);
  lv_obj_set_size(body, 40, 18);
  lv_obj_set_style_bg_color(body, color(0x111111), 0);
  lv_obj_set_style_bg_grad_color(body, color(0xF0A12A), 0);
  lv_obj_set_style_bg_grad_dir(body, LV_GRAD_DIR_HOR, 0);
  lv_obj_set_style_radius(body, 2, 0);
  lv_obj_t* terminal = lv_obj_create(parent);
  lv_obj_remove_style_all(terminal);
  lv_obj_set_pos(terminal, 66, y + 5);
  lv_obj_set_size(terminal, 5, 8);
  lv_obj_set_style_bg_color(terminal, color(0xE6C067), 0);
}

void createBulbIcon(lv_obj_t* parent, int y) {
  lv_obj_t* glass = makeCircle(parent, 45, y + 12, 28, 0xFFFFFF, 0x808080, 1);
  lv_obj_set_style_bg_opa(glass, LV_OPA_40, 0);
  lv_obj_t* base = lv_obj_create(parent);
  lv_obj_remove_style_all(base);
  lv_obj_set_pos(base, 38, y + 24);
  lv_obj_set_size(base, 14, 12);
  lv_obj_set_style_bg_color(base, color(0x666666), 0);
  static lv_point_precise_t filamentPoints[] = {{38, 15}, {45, 24}, {45, 15}, {52, 15}};
  lv_obj_t* filament = lv_line_create(parent);
  lv_line_set_points(filament, filamentPoints, 4);
  lv_obj_set_style_line_color(filament, color(0xD85735), 0);
  lv_obj_set_style_line_width(filament, 2, 0);
}

void createResistorIcon(lv_obj_t* parent, int y) {
  lv_obj_t* body = lv_obj_create(parent);
  lv_obj_remove_style_all(body);
  lv_obj_set_pos(body, 24, y);
  lv_obj_set_size(body, 42, 18);
  lv_obj_set_style_radius(body, 9, 0);
  lv_obj_set_style_bg_color(body, color(0xD6B77A), 0);
  lv_obj_set_style_border_color(body, color(0xB99554), 0);
  lv_obj_set_style_border_width(body, 1, 0);
  const int xs[] = {32, 40, 49};
  const uint32_t cols[] = {0x8C4F16, 0x111111, 0xD7A900};
  for (int i = 0; i < 3; ++i) {
    lv_obj_t* band = lv_obj_create(parent);
    lv_obj_remove_style_all(band);
    lv_obj_set_pos(band, xs[i], y);
    lv_obj_set_size(band, 4, 18);
    lv_obj_set_style_bg_color(band, color(cols[i]), 0);
  }
}

void createSwitchIcon(lv_obj_t* parent, int y) {
  static lv_point_precise_t leftLead[] = {{18, 18}, {34, 18}};
  static lv_point_precise_t lever[] = {{36, 18}, {58, 0}};
  static lv_point_precise_t rightLead[] = {{60, 18}, {72, 18}};
  lv_obj_t* l1 = lv_line_create(parent);
  lv_line_set_points(l1, leftLead, 2);
  lv_obj_set_style_line_color(l1, color(0xA96B5A), 0);
  lv_obj_set_style_line_width(l1, 7, 0);
  lv_obj_set_style_line_rounded(l1, true, 0);
  lv_obj_set_pos(l1, 0, y);
  lv_obj_t* l2 = lv_line_create(parent);
  lv_line_set_points(l2, lever, 2);
  lv_obj_set_style_line_color(l2, color(0xA96B5A), 0);
  lv_obj_set_style_line_width(l2, 6, 0);
  lv_obj_set_style_line_rounded(l2, true, 0);
  lv_obj_set_pos(l2, 0, y);
  lv_obj_t* l3 = lv_line_create(parent);
  lv_line_set_points(l3, rightLead, 2);
  lv_obj_set_style_line_color(l3, color(0xA96B5A), 0);
  lv_obj_set_style_line_width(l3, 7, 0);
  lv_obj_set_style_line_rounded(l3, true, 0);
  lv_obj_set_pos(l3, 0, y);
  makeCircle(parent, 35, y + 18, 14, 0xB7B7B7, 0x111111, 3);
}

void selectTool(Tool tool) {
  selectedTool = tool;
  if (wireToolTile != nullptr) {
    lv_obj_set_style_bg_color(wireToolTile, color(tool == Tool::Wire ? 0xFFFFFF : 0xF3F3F3), 0);
    lv_obj_set_style_border_color(wireToolTile, color(tool == Tool::Wire ? 0x4C7FD9 : 0xD0D0D0), 0);
  }
}

void toolEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) {
    return;
  }
  Tool* tool = static_cast<Tool*>(lv_event_get_user_data(event));
  if (tool != nullptr) {
    selectTool(*tool);
  }
}

lv_obj_t* createToolTile(lv_obj_t* sidebar, const char* name, Tool tool, int y) {
  lv_obj_t* tile = lv_obj_create(sidebar);
  stylePanel(tile, 0xF3F3F3, 0xD0D0D0, 1);
  lv_obj_set_pos(tile, 0, y);
  lv_obj_set_size(tile, SIDEBAR_W, 50);
  lv_obj_add_flag(tile, LV_OBJ_FLAG_CLICKABLE);
  static Tool toolValues[] = {Tool::Wire, Tool::Battery, Tool::Bulb, Tool::Resistor, Tool::Switch};
  lv_obj_add_event_cb(tile, toolEvent, LV_EVENT_CLICKED, &toolValues[static_cast<int>(tool)]);

  if (tool == Tool::Wire) createWireIcon(tile, 17);
  if (tool == Tool::Battery) createBatteryIcon(tile, 14);
  if (tool == Tool::Bulb) createBulbIcon(tile, 4);
  if (tool == Tool::Resistor) createResistorIcon(tile, 14);
  if (tool == Tool::Switch) createSwitchIcon(tile, 14);
  makeLabel(tile, name, 34, &lv_font_montserrat_14);
  return tile;
}

void updateWire(int x0, int y0, int x1, int y1) {
  if (wireLine == nullptr) {
    wireLine = lv_line_create(canvasArea);
    lv_obj_set_size(wireLine, CANVAS_W, CANVAS_H);
    lv_obj_set_pos(wireLine, 0, 0);
    lv_obj_set_style_line_color(wireLine, color(0x8A4F3D), 0);
    lv_obj_set_style_line_width(wireLine, 12, 0);
    lv_obj_set_style_line_rounded(wireLine, true, 0);
    lv_obj_move_foreground(wireLine);
  }

  wirePoints[0] = {static_cast<lv_coord_t>(x0), static_cast<lv_coord_t>(y0)};
  wirePoints[1] = {static_cast<lv_coord_t>(x1), static_cast<lv_coord_t>(y1)};
  lv_line_set_points_mutable(wireLine, wirePoints, 2);

  int mx = (x0 + x1) / 2;
  int my = (y0 + y1) / 2;
  if (startNode == nullptr) {
    startTarget = makeCircle(canvasArea, x0, y0, 30, 0xFFFFFF, 0xC02A20, 2);
    endTarget = makeCircle(canvasArea, x1, y1, 30, 0xFFFFFF, 0xC02A20, 2);
    startNode = makeCircle(canvasArea, x0, y0, 16, 0x7DB9DA, 0xFFFFFF, 1);
    midNode = makeCircle(canvasArea, mx, my, 16, 0x7DB9DA, 0xFFFFFF, 1);
    endNode = makeCircle(canvasArea, x1, y1, 16, 0x7DB9DA, 0xFFFFFF, 1);
  }

  setCircleCenter(startTarget, x0, y0);
  setCircleCenter(endTarget, x1, y1);
  setCircleCenter(startNode, x0, y0);
  setCircleCenter(midNode, mx, my);
  setCircleCenter(endNode, x1, y1);

  lv_obj_move_foreground(startTarget);
  lv_obj_move_foreground(endTarget);
  lv_obj_move_foreground(startNode);
  lv_obj_move_foreground(midNode);
  lv_obj_move_foreground(endNode);
}

bool eventPoint(lv_event_t* event, lv_point_t* out) {
  lv_indev_t* indev = lv_event_get_indev(event);
  if (indev == nullptr) {
    return false;
  }
  lv_indev_get_point(indev, out);
  out->x -= CANVAS_X;
  if (out->x < 0) out->x = 0;
  if (out->x >= CANVAS_W) out->x = CANVAS_W - 1;
  if (out->y < 0) out->y = 0;
  if (out->y >= CANVAS_H) out->y = CANVAS_H - 1;
  return true;
}

void canvasEvent(lv_event_t* event) {
  if (selectedTool != Tool::Wire) {
    return;
  }

  lv_event_code_t code = lv_event_get_code(event);
  lv_point_t point{};
  if (!eventPoint(event, &point)) {
    return;
  }

  if (code == LV_EVENT_PRESSED) {
    drawingWire = true;
    updateWire(point.x, point.y, point.x + 1, point.y);
  } else if (code == LV_EVENT_PRESSING && drawingWire) {
    updateWire(wirePoints[0].x, wirePoints[0].y, point.x, point.y);
  } else if ((code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) && drawingWire) {
    drawingWire = false;
    updateWire(wirePoints[0].x, wirePoints[0].y, point.x, point.y);
  }
}

void createChevron(lv_obj_t* parent, bool up) {
  lv_obj_t* label = lv_label_create(parent);
  lv_label_set_text(label, up ? "^" : "v");
  lv_obj_set_style_text_color(label, color(up ? 0x808080 : 0x000000), 0);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
  lv_obj_center(label);
}

void createBottomDock(lv_obj_t* screen) {
  lv_obj_t* wireButton = lv_obj_create(screen);
  stylePanel(wireButton, 0xEDEDED, 0x111111, 1);
  lv_obj_set_pos(wireButton, 100, 284);
  lv_obj_set_size(wireButton, 36, 28);
  lv_obj_set_style_radius(wireButton, 4, 0);
  createWireIcon(wireButton, 12);

  lv_obj_t* ghostButton = lv_obj_create(screen);
  stylePanel(ghostButton, 0xD3DCF2, 0x7080A0, 1);
  lv_obj_set_pos(ghostButton, 146, 284);
  lv_obj_set_size(ghostButton, 36, 28);
  lv_obj_set_style_radius(ghostButton, 4, 0);
  static lv_point_precise_t plusH[] = {{9, 14}, {27, 14}};
  static lv_point_precise_t plusV[] = {{18, 7}, {18, 21}};
  lv_obj_t* h = lv_line_create(ghostButton);
  lv_line_set_points(h, plusH, 2);
  lv_obj_set_style_line_color(h, color(0x586070), 0);
  lv_obj_set_style_line_width(h, 2, 0);
  lv_obj_t* v = lv_line_create(ghostButton);
  lv_line_set_points(v, plusV, 2);
  lv_obj_set_style_line_color(v, color(0x586070), 0);
  lv_obj_set_style_line_width(v, 2, 0);
}

} // namespace

void CircuitLabApp::create() {
  lv_obj_t* screen = lv_obj_create(nullptr);
  lv_scr_load(screen);
  stylePanel(screen, 0xA8C0F2, 0xA8C0F2, 0);

  canvasArea = lv_obj_create(screen);
  stylePanel(canvasArea, 0xA8C0F2, 0xA8C0F2, 0);
  lv_obj_set_pos(canvasArea, CANVAS_X, 0);
  lv_obj_set_size(canvasArea, CANVAS_W, CANVAS_H);
  lv_obj_add_flag(canvasArea, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(canvasArea, canvasEvent, LV_EVENT_PRESSED, nullptr);
  lv_obj_add_event_cb(canvasArea, canvasEvent, LV_EVENT_PRESSING, nullptr);
  lv_obj_add_event_cb(canvasArea, canvasEvent, LV_EVENT_RELEASED, nullptr);
  lv_obj_add_event_cb(canvasArea, canvasEvent, LV_EVENT_PRESS_LOST, nullptr);

  lv_obj_t* sidebar = lv_obj_create(screen);
  stylePanel(sidebar, 0xEFEFEF, 0x222222, 2);
  lv_obj_set_pos(sidebar, 0, 0);
  lv_obj_set_size(sidebar, SIDEBAR_W, SCREEN_H - BOTTOM_TRAY_H);

  lv_obj_t* top = lv_obj_create(sidebar);
  stylePanel(top, 0xEFEFEF, 0xB8B8B8, 1);
  lv_obj_set_pos(top, 0, 0);
  lv_obj_set_size(top, SIDEBAR_W, 26);
  createChevron(top, true);

  wireToolTile = createToolTile(sidebar, "Wire", Tool::Wire, 26);
  createToolTile(sidebar, "Battery", Tool::Battery, 76);
  createToolTile(sidebar, "Light Bulb", Tool::Bulb, 126);
  createToolTile(sidebar, "Resistor", Tool::Resistor, 176);
  createToolTile(sidebar, "Switch", Tool::Switch, 226);

  lv_obj_t* bottom = lv_obj_create(screen);
  stylePanel(bottom, 0xD9D9D9, 0x222222, 2);
  lv_obj_set_pos(bottom, 0, SCREEN_H - BOTTOM_TRAY_H);
  lv_obj_set_size(bottom, SIDEBAR_W, BOTTOM_TRAY_H);
  createChevron(bottom, false);

  createBottomDock(screen);
  selectTool(Tool::Wire);
}

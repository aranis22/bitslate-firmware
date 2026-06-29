#include "CircuitLabApp.h"

#include <lvgl.h>

#include "CircuitModel.h"

namespace {

constexpr int SCREEN_W = 480;
constexpr int SCREEN_H = 320;
constexpr int SIDEBAR_W = 90;
constexpr int CANVAS_X = SIDEBAR_W;
constexpr int CANVAS_W = SCREEN_W - SIDEBAR_W;
constexpr int CANVAS_H = SCREEN_H;
constexpr int SNAP_DISTANCE = 22;

enum class Tool {
  Battery,
  Bulb,
  Wire,
  Clear
};

CircuitModel model;
Tool selectedTool = Tool::Battery;
lv_obj_t* canvasArea = nullptr;
lv_obj_t* statusLabel = nullptr;
lv_obj_t* toolTiles[4] = {};
lv_point_precise_t wirePoints[CircuitModel::MaxWires][2] = {};
int firstWireTerminalId = 0;
const char* statusText = "Select a tool";

void renderCanvas();
void makeToolTileChildrenClickable(lv_obj_t* tile, Tool* toolValue);

lv_color_t color(uint32_t hex) {
  return lv_color_hex(hex);
}

void clearInteractionFlags(lv_obj_t* obj) {
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
  clearInteractionFlags(obj);
}

lv_obj_t* makeLabel(lv_obj_t* parent, const char* text, int x, int y, const lv_font_t* font = &lv_font_montserrat_14) {
  lv_obj_t* label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_obj_set_pos(label, x, y);
  lv_obj_set_style_text_color(label, color(0x111111), 0);
  lv_obj_set_style_text_font(label, font, 0);
  clearInteractionFlags(label);
  return label;
}

lv_obj_t* makeCircle(lv_obj_t* parent, int cx, int cy, int size, uint32_t fill, uint32_t border, int borderWidth) {
  lv_obj_t* circle = lv_obj_create(parent);
  lv_obj_remove_style_all(circle);
  lv_obj_set_size(circle, size, size);
  lv_obj_set_pos(circle, cx - size / 2, cy - size / 2);
  lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(circle, color(fill), 0);
  lv_obj_set_style_bg_opa(circle, LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(circle, color(border), 0);
  lv_obj_set_style_border_width(circle, borderWidth, 0);
  clearInteractionFlags(circle);
  return circle;
}

void updateStatus(const char* text) {
  statusText = text;
  if (statusLabel != nullptr) {
    lv_label_set_text(statusLabel, statusText);
    lv_obj_set_style_text_color(statusLabel, color(model.isBulbLit() ? 0x0A7A31 : 0x203040), 0);
  }
}

void updateCircuitStatus() {
  updateStatus(model.isBulbLit() ? "Circuit complete: bulb ON" : "Circuit incomplete");
}

void refreshToolTiles() {
  for (int i = 0; i < 4; ++i) {
    if (toolTiles[i] == nullptr) {
      continue;
    }

    bool active = static_cast<int>(selectedTool) == i;
    lv_obj_set_style_bg_color(toolTiles[i], color(active ? 0xDDEBFF : 0xF7F7F7), 0);
    lv_obj_set_style_border_color(toolTiles[i], color(active ? 0x3A73C8 : 0xC8C8C8), 0);
  }
}

void selectTool(Tool tool) {
  if (tool == Tool::Clear) {
    model.clear();
    firstWireTerminalId = 0;
    selectedTool = Tool::Battery;
    refreshToolTiles();
    updateStatus("Select a tool");
    renderCanvas();
    return;
  }

  selectedTool = tool;
  firstWireTerminalId = 0;
  refreshToolTiles();

  if (tool == Tool::Battery) updateStatus("Place battery");
  if (tool == Tool::Bulb) updateStatus("Place bulb");
  if (tool == Tool::Wire) updateStatus("Tap first terminal");
}

void drawBatteryIcon(lv_obj_t* parent, int x, int y) {
  lv_obj_t* neg = lv_obj_create(parent);
  styleBox(neg, 0x151515, 0x151515, 0, 2);
  lv_obj_set_pos(neg, x, y);
  lv_obj_set_size(neg, 27, 16);

  lv_obj_t* pos = lv_obj_create(parent);
  styleBox(pos, 0xECA43A, 0xECA43A, 0, 2);
  lv_obj_set_pos(pos, x + 27, y);
  lv_obj_set_size(pos, 27, 16);

  lv_obj_t* cap = lv_obj_create(parent);
  styleBox(cap, 0xF4D079, 0xF4D079, 0, 1);
  lv_obj_set_pos(cap, x + 54, y + 4);
  lv_obj_set_size(cap, 5, 8);
}

void drawBulbIcon(lv_obj_t* parent, int cx, int cy, bool lit) {
  makeCircle(parent, cx, cy, lit ? 38 : 32, lit ? 0xFFE44A : 0xECE7C8, 0x777777, 2);
  lv_obj_t* base = lv_obj_create(parent);
  styleBox(base, 0x888888, 0x666666, 1, 2);
  lv_obj_set_pos(base, cx - 12, cy + 18);
  lv_obj_set_size(base, 24, 14);

  static lv_point_precise_t filament[] = {{0, 0}, {8, 12}, {16, 0}};
  lv_obj_t* line = lv_line_create(parent);
  lv_line_set_points(line, filament, 3);
  lv_obj_set_pos(line, cx - 8, cy - 2);
  lv_obj_set_style_line_color(line, color(0xD75A32), 0);
  lv_obj_set_style_line_width(line, 3, 0);
}

void drawBulbTrayIcon(lv_obj_t* parent, int cx, int cy) {
  makeCircle(parent, cx, cy, 24, 0xECE7C8, 0x777777, 2);

  lv_obj_t* base = lv_obj_create(parent);
  styleBox(base, 0x888888, 0x666666, 1, 2);
  lv_obj_set_pos(base, cx - 9, cy + 11);
  lv_obj_set_size(base, 18, 8);

  static lv_point_precise_t filament[] = {{0, 0}, {6, 8}, {12, 0}};
  lv_obj_t* line = lv_line_create(parent);
  lv_line_set_points(line, filament, 3);
  lv_obj_set_pos(line, cx - 6, cy - 1);
  lv_obj_set_style_line_color(line, color(0xD75A32), 0);
  lv_obj_set_style_line_width(line, 2, 0);
}

void drawWireIcon(lv_obj_t* parent, int x, int y) {
  static lv_point_precise_t points[] = {{0, 0}, {54, 0}};
  lv_obj_t* line = lv_line_create(parent);
  lv_line_set_points(line, points, 2);
  lv_obj_set_pos(line, x, y);
  lv_obj_set_style_line_color(line, color(0x7A4A32), 0);
  lv_obj_set_style_line_width(line, 7, 0);
  lv_obj_set_style_line_rounded(line, true, 0);
}

void toolEvent(lv_event_t* event) {
  lv_event_code_t code = lv_event_get_code(event);
  if (code != LV_EVENT_CLICKED && code != LV_EVENT_PRESSED) {
    return;
  }

  Tool* tool = static_cast<Tool*>(lv_event_get_user_data(event));
  if (tool != nullptr) {
    selectTool(*tool);
    if (toolTiles[static_cast<int>(*tool)] != nullptr || *tool == Tool::Clear) {
      // Redraw is handled by the main render path.
    }
  }
}

lv_obj_t* createToolTile(lv_obj_t* sidebar, Tool tool, const char* label, int y) {
  lv_obj_t* tile = lv_obj_create(sidebar);
  styleBox(tile, 0xF7F7F7, 0xC8C8C8, 1, 4);
  lv_obj_set_pos(tile, 6, y);
  lv_obj_set_size(tile, SIDEBAR_W - 12, 58);
  lv_obj_add_flag(tile, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(tile, LV_OBJ_FLAG_SCROLLABLE);

  static Tool toolValues[] = {Tool::Battery, Tool::Bulb, Tool::Wire, Tool::Clear};
  Tool* toolValue = &toolValues[static_cast<int>(tool)];
  lv_obj_add_event_cb(tile, toolEvent, LV_EVENT_PRESSED, toolValue);
  lv_obj_add_event_cb(tile, toolEvent, LV_EVENT_CLICKED, toolValue);
  toolTiles[static_cast<int>(tool)] = tile;

  if (tool == Tool::Battery) drawBatteryIcon(tile, 10, 10);
  if (tool == Tool::Bulb) drawBulbTrayIcon(tile, 39, 14);
  if (tool == Tool::Wire) drawWireIcon(tile, 12, 18);
  if (tool == Tool::Clear) makeLabel(tile, "Reset", 20, 12, &lv_font_montserrat_18);

  lv_obj_t* text = lv_label_create(tile);
  lv_label_set_text(text, label);
  lv_obj_set_style_text_color(text, color(0x111111), 0);
  lv_obj_set_style_text_font(text, &lv_font_montserrat_14, 0);
  lv_obj_align(text, LV_ALIGN_BOTTOM_MID, 0, -3);
  clearInteractionFlags(text);
  makeToolTileChildrenClickable(tile, toolValue);
  return tile;
}

void makeToolTileChildrenClickable(lv_obj_t* tile, Tool* toolValue) {
  uint32_t childCount = lv_obj_get_child_count(tile);
  for (uint32_t i = 0; i < childCount; ++i) {
    lv_obj_t* child = lv_obj_get_child(tile, i);
    lv_obj_add_flag(child, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(child, toolEvent, LV_EVENT_PRESSED, toolValue);
    lv_obj_add_event_cb(child, toolEvent, LV_EVENT_CLICKED, toolValue);
  }
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

void drawTerminal(int terminalId) {
  int x = 0;
  int y = 0;
  if (!model.getTerminalWorldPos(terminalId, &x, &y)) {
    return;
  }

  makeCircle(canvasArea, x, y, 15, 0xF3FBFF, 0x4E8DB0, 2);
}

void renderWires() {
  for (int i = 0; i < model.getWireCount(); ++i) {
    const Wire* wire = model.getWireAt(i);
    if (wire == nullptr) {
      continue;
    }

    int x0 = 0;
    int y0 = 0;
    int x1 = 0;
    int y1 = 0;
    if (!model.getTerminalWorldPos(wire->fromTerminalId, &x0, &y0) ||
        !model.getTerminalWorldPos(wire->toTerminalId, &x1, &y1)) {
      continue;
    }

    wirePoints[i][0] = {static_cast<lv_coord_t>(x0), static_cast<lv_coord_t>(y0)};
    wirePoints[i][1] = {static_cast<lv_coord_t>(x1), static_cast<lv_coord_t>(y1)};
    lv_obj_t* line = lv_line_create(canvasArea);
    lv_line_set_points_mutable(line, wirePoints[i], 2);
    lv_obj_set_style_line_color(line, color(0x7A4A32), 0);
    lv_obj_set_style_line_width(line, 8, 0);
    lv_obj_set_style_line_rounded(line, true, 0);
  }
}

void renderBattery(const Component& component) {
  drawBatteryIcon(canvasArea, component.x - 35, component.y - 10);
  makeLabel(canvasArea, "-", component.x - 26, component.y - 9, &lv_font_montserrat_18);
  makeLabel(canvasArea, "+", component.x + 15, component.y - 9, &lv_font_montserrat_18);
  drawTerminal(component.terminalIds[0]);
  drawTerminal(component.terminalIds[1]);
}

void renderBulb(const Component& component) {
  drawBulbIcon(canvasArea, component.x, component.y, model.isBulbLit(component.id));
  drawTerminal(component.terminalIds[0]);
  drawTerminal(component.terminalIds[1]);
}

void renderCanvas() {
  if (canvasArea == nullptr) {
    return;
  }

  lv_obj_clean(canvasArea);
  renderWires();

  for (int i = 0; i < model.getComponentCount(); ++i) {
    const Component* component = model.getComponentAt(i);
    if (component == nullptr) {
      continue;
    }

    if (component->type == ComponentType::Battery) {
      renderBattery(*component);
    } else {
      renderBulb(*component);
    }
  }

  if (firstWireTerminalId != 0) {
    int x = 0;
    int y = 0;
    if (model.getTerminalWorldPos(firstWireTerminalId, &x, &y)) {
      makeCircle(canvasArea, x, y, 25, 0xFFF1F1, 0xCC2B20, 2);
    }
  }
}

void canvasEvent(lv_event_t* event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) {
    return;
  }

  lv_point_t point{};
  if (!eventPoint(event, &point)) {
    return;
  }

  if (selectedTool == Tool::Battery) {
    model.addComponent(ComponentType::Battery, point.x, point.y);
    updateCircuitStatus();
    renderCanvas();
    return;
  }

  if (selectedTool == Tool::Bulb) {
    model.addComponent(ComponentType::Bulb, point.x, point.y);
    updateCircuitStatus();
    renderCanvas();
    return;
  }

  if (selectedTool == Tool::Wire) {
    int terminalId = model.findNearestTerminal(point.x, point.y, SNAP_DISTANCE);
    if (terminalId == 0) {
      updateStatus(firstWireTerminalId == 0 ? "Tap first terminal" : "Tap second terminal");
      return;
    }

    if (firstWireTerminalId == 0) {
      firstWireTerminalId = terminalId;
      updateStatus("Tap second terminal");
      renderCanvas();
      return;
    }

    if (model.addWire(firstWireTerminalId, terminalId)) {
      firstWireTerminalId = 0;
      updateCircuitStatus();
    } else {
      firstWireTerminalId = 0;
      updateStatus("Circuit incomplete");
    }
    renderCanvas();
  }
}

} // namespace

void CircuitLabApp::create() {
  model.clear();
  firstWireTerminalId = 0;
  selectedTool = Tool::Battery;

  lv_obj_t* screen = lv_obj_create(nullptr);
  lv_scr_load(screen);
  styleBox(screen, 0xA8C0F2, 0xA8C0F2, 0);

  canvasArea = lv_obj_create(screen);
  styleBox(canvasArea, 0xA8C0F2, 0xA8C0F2, 0);
  lv_obj_set_pos(canvasArea, CANVAS_X, 0);
  lv_obj_set_size(canvasArea, CANVAS_W, CANVAS_H);
  lv_obj_add_flag(canvasArea, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(canvasArea, canvasEvent, LV_EVENT_CLICKED, nullptr);

  lv_obj_t* sidebar = lv_obj_create(screen);
  styleBox(sidebar, 0xEFEFEF, 0x222222, 2);
  lv_obj_set_pos(sidebar, 0, 0);
  lv_obj_set_size(sidebar, SIDEBAR_W, SCREEN_H);

  makeLabel(sidebar, "Circuit", 14, 8, &lv_font_montserrat_18);
  createToolTile(sidebar, Tool::Battery, "Battery", 38);
  createToolTile(sidebar, Tool::Bulb, "Bulb", 101);
  createToolTile(sidebar, Tool::Wire, "Wire", 164);
  createToolTile(sidebar, Tool::Clear, "Clear", 227);

  lv_obj_t* statusBox = lv_obj_create(screen);
  styleBox(statusBox, 0xEAF1FF, 0x8FA5C9, 1, 5);
  lv_obj_set_pos(statusBox, CANVAS_X + 8, SCREEN_H - 31);
  lv_obj_set_size(statusBox, 240, 24);

  statusLabel = lv_label_create(statusBox);
  lv_obj_set_pos(statusLabel, 8, 4);
  lv_obj_set_style_text_font(statusLabel, &lv_font_montserrat_14, 0);
  clearInteractionFlags(statusLabel);

  refreshToolTiles();
  updateStatus("Select a tool");
  renderCanvas();
}

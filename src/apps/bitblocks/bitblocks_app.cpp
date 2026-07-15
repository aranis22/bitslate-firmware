#include "bitblocks_app.h"

#include <Arduino.h>
#include <cstdint>
#include <lvgl.h>

LV_FONT_DECLARE(monogram_16);
LV_FONT_DECLARE(monogram_19);
LV_FONT_DECLARE(monogram_20);
LV_FONT_DECLARE(monogram_24);

namespace {
lv_obj_t* root = nullptr;
lv_obj_t* categoryHeader = nullptr;
lv_obj_t* categoryLabel = nullptr;
lv_obj_t* palette = nullptr;
lv_obj_t* paletteInputLayer = nullptr;
lv_obj_t* categoryDropdown = nullptr;
lv_obj_t* workspace = nullptr;
lv_obj_t* playButton = nullptr;
lv_obj_t* stopButton = nullptr;

enum class Category : uint8_t { Movement, Events, Control, Operators, Camera };
Category selectedCategory = Category::Movement;

struct CategoryStyle {
  const char* name;
  const char* icon;
  uint32_t fill;
  uint32_t outline;
};

constexpr CategoryStyle categoryStyles[] = {
  {"MOVEMENT", "+", 0x4D69B2, 0x29365F},
  {"EVENTS", "!", 0xE3B43E, 0x836619},
  {"CONTROL", "{}", 0xCE762D, 0x754119},
  {"OPERATORS", "+-", 0x639A3B, 0x355A22},
  {"CAMERA", "[o]", 0x6945A1, 0x3B2860},
};

struct BlockSpec {
  const char* label;
  uint32_t fill;
  uint32_t outline;
  int width;
};

struct WorkspaceBlock {
  lv_obj_t* object = nullptr;
  lv_obj_t* selection = nullptr;
  BlockSpec ownedSpec = {};
  const BlockSpec* spec = nullptr;
  WorkspaceBlock* above = nullptr;
  WorkspaceBlock* below = nullptr;
  int dragOffsetX = 0;
  int dragOffsetY = 0;
  lv_point_t pressPoint = {};
  uint32_t pressTick = 0;
  bool dragging = false;
  bool active = false;
};

struct PaletteBlockState {
  lv_obj_t* object = nullptr;
  lv_obj_t* artwork = nullptr;
  lv_obj_t* selection = nullptr;
  const BlockSpec* spec = nullptr;
  uint8_t id = 0;
  lv_area_t hitRegion = {};
};

constexpr int kMaxWorkspaceBlocks = 24;
WorkspaceBlock workspaceBlocks[kMaxWorkspaceBlocks];
WorkspaceBlock* selectedBlock = nullptr;
PaletteBlockState paletteBlockStates[4];
PaletteBlockState* selectedPaletteBlock = nullptr;

BlockSpec paletteSpecs[4];
uint8_t paletteSpecCount = 0;
uint8_t palettePage = 0;

constexpr uint8_t kBlocksPerPage = 3;

lv_color_t color(uint32_t hex) { return lv_color_hex(hex); }

void base(lv_obj_t* obj, uint32_t fill, uint32_t border = 0, int borderWidth = 0) {
  lv_obj_remove_style_all(obj);
  lv_obj_set_style_bg_color(obj, color(fill), 0);
  lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(obj, color(border), 0);
  lv_obj_set_style_border_width(obj, borderWidth, 0);
  lv_obj_set_style_radius(obj, 0, 0);
  lv_obj_set_style_pad_all(obj, 0, 0);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

lv_obj_t* panel(lv_obj_t* parent, int x, int y, int w, int h, uint32_t fill, uint32_t border, int bw = 2) {
  lv_obj_t* obj = lv_obj_create(parent); base(obj, fill, border, bw); lv_obj_set_pos(obj, x, y); lv_obj_set_size(obj, w, h); return obj;
}

lv_obj_t* text(lv_obj_t* parent, const char* value, int x, int y, const lv_font_t* font, uint32_t ink) {
  lv_obj_t* label = lv_label_create(parent);lv_label_set_text(label, value);lv_obj_set_pos(label, x, y);
  lv_obj_clear_flag(label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_set_style_text_font(label, font, 0);lv_obj_set_style_text_color(label, color(ink), 0);return label;
}

lv_obj_t* create_block_visual(lv_obj_t* parent, int x, int y, int w, uint32_t fill, uint32_t outline,
                              uint32_t surrounding, const char* label) {
  // Build the silhouette from integer-aligned rectangles.  The dark union is
  // the outline; smaller fill pieces create stepped/chamfered ends without
  // introducing internal seams or an external shadow.
  lv_obj_t* block = panel(parent, x, y, w, 37, surrounding, surrounding, 0);
  lv_obj_set_style_bg_opa(block, LV_OPA_TRANSP, 0);
  panel(block, 4, 0, w - 8, 31, outline, outline, 0);
  panel(block, 0, 4, w, 23, outline, outline, 0);
  panel(block, 4, 3, w - 8, 25, fill, fill, 0);
  panel(block, 3, 7, w - 6, 17, fill, fill, 0);

  // Shared inward top notch and outward bottom connector.
  panel(block, 20, 0, 25, 6, surrounding, surrounding, 0);
  panel(block, 20, 4, 4, 4, outline, outline, 0);
  panel(block, 41, 4, 4, 4, outline, outline, 0);
  panel(block, 24, 5, 17, 3, outline, outline, 0);
  panel(block, 25, 5, 15, 2, surrounding, surrounding, 0);
  panel(block, 24, 27, 22, 9, outline, outline, 0);
  panel(block, 27, 27, 16, 6, fill, fill, 0);

  text(block, label, 10, 8, &monogram_19, 0xFFFFFF);
  return block;
}

bool pointer_position(lv_point_t* point) {
  lv_indev_t* indev = lv_indev_active();
  if(indev == nullptr) return false;
  lv_indev_get_point(indev, point);
  return true;
}

bool is_in_stack(WorkspaceBlock* rootBlock, WorkspaceBlock* candidate) {
  for(WorkspaceBlock* block = rootBlock; block != nullptr; block = block->below) {
    if(block == candidate) return true;
  }
  return false;
}

void move_stack_by(WorkspaceBlock* block, int dx, int dy) {
  for(WorkspaceBlock* current = block; current != nullptr; current = current->below) {
    lv_obj_set_pos(current->object, lv_obj_get_x(current->object) + dx, lv_obj_get_y(current->object) + dy);
    lv_obj_move_foreground(current->object);
  }
  if(playButton != nullptr) lv_obj_move_foreground(playButton);
  if(stopButton != nullptr) lv_obj_move_foreground(stopButton);
}

void clear_selection() {
  if(selectedBlock != nullptr && selectedBlock->selection != nullptr) {
    lv_obj_delete(selectedBlock->selection);
    selectedBlock->selection = nullptr;
  }
  if(selectedPaletteBlock != nullptr && selectedPaletteBlock->selection != nullptr) {
    lv_obj_delete(selectedPaletteBlock->selection);
    selectedPaletteBlock->selection = nullptr;
  }
  selectedBlock = nullptr;
  selectedPaletteBlock = nullptr;
}

void add_horizontal_dashes(lv_obj_t* overlay, int x1, int x2, int y) {
  for(int x = x1; x < x2; x += 6) panel(overlay, x, y, 3, 2, 0xFFD83D, 0xFFD83D, 0);
}

void add_vertical_dashes(lv_obj_t* overlay, int x, int y1, int y2) {
  for(int y = y1; y < y2; y += 6) panel(overlay, x, y, 2, 3, 0xFFD83D, 0xFFD83D, 0);
}

lv_obj_t* create_selection_overlay(lv_obj_t* object, int width) {
  lv_obj_t* overlay = panel(object, 0, 0, width, 37, 0xFFFFFF, 0xFFFFFF, 0);
  lv_obj_set_style_bg_opa(overlay, LV_OPA_TRANSP, 0);
  add_horizontal_dashes(overlay, 4, 20, 0);
  add_horizontal_dashes(overlay, 45, width - 4, 0);
  add_horizontal_dashes(overlay, 4, 24, 27);
  add_horizontal_dashes(overlay, 46, width - 4, 27);
  add_horizontal_dashes(overlay, 24, 46, 35);
  add_horizontal_dashes(overlay, 24, 45, 7);
  add_vertical_dashes(overlay, 0, 4, 27);
  add_vertical_dashes(overlay, width - 2, 4, 27);
  add_vertical_dashes(overlay, 24, 28, 35);
  add_vertical_dashes(overlay, 44, 28, 35);
  return overlay;
}

void select_block(WorkspaceBlock* block) {
  if(selectedBlock == block && selectedPaletteBlock == nullptr) return;
  clear_selection();
  selectedBlock = block;
  block->selection = create_selection_overlay(block->object, block->spec->width);
}

void select_palette_block(PaletteBlockState* block) {
  if(selectedPaletteBlock == block && selectedBlock == nullptr) return;
  clear_selection();
  selectedPaletteBlock = block;
  block->selection = create_selection_overlay(block->artwork, block->spec->width);
}

WorkspaceBlock* allocate_workspace_block(const BlockSpec* spec, int x, int y);

void snap_stack(WorkspaceBlock* dragged) {
  const int draggedX = lv_obj_get_x(dragged->object);
  const int draggedY = lv_obj_get_y(dragged->object);

  for(WorkspaceBlock& candidate : workspaceBlocks) {
    if(!candidate.active || is_in_stack(dragged, &candidate)) continue;
    const int candidateX = lv_obj_get_x(candidate.object);
    const int candidateY = lv_obj_get_y(candidate.object);
    if(candidate.below == nullptr && LV_ABS(draggedX - candidateX) <= 24 &&
       LV_ABS(draggedY - (candidateY + 31)) <= 12) {
      const int dx = candidateX - draggedX;
      const int dy = candidateY + 31 - draggedY;
      candidate.below = dragged;
      dragged->above = &candidate;
      move_stack_by(dragged, dx, dy);
      return;
    }
  }

  WorkspaceBlock* bottom = dragged;
  while(bottom->below != nullptr) bottom = bottom->below;
  const int bottomX = lv_obj_get_x(bottom->object);
  const int bottomY = lv_obj_get_y(bottom->object);
  for(WorkspaceBlock& candidate : workspaceBlocks) {
    if(!candidate.active || candidate.above != nullptr || is_in_stack(dragged, &candidate)) continue;
    const int candidateX = lv_obj_get_x(candidate.object);
    const int candidateY = lv_obj_get_y(candidate.object);
    if(LV_ABS(bottomX - candidateX) <= 24 && LV_ABS(candidateY - (bottomY + 31)) <= 12) {
      bottom->below = &candidate;
      candidate.above = bottom;
      move_stack_by(&candidate, bottomX - candidateX, bottomY + 31 - candidateY);
      return;
    }
  }
}

void workspace_block_event(lv_event_t* event) {
  WorkspaceBlock* block = static_cast<WorkspaceBlock*>(lv_event_get_user_data(event));
  if(lv_event_get_code(event) == LV_EVENT_PRESSED) select_block(block);
}

WorkspaceBlock* allocate_workspace_block(const BlockSpec* spec, int x, int y) {
  for(WorkspaceBlock& block : workspaceBlocks) {
    if(block.active) continue;
    block = WorkspaceBlock{};
    block.active = true;
    block.ownedSpec = *spec;
    block.spec = &block.ownedSpec;
    block.object = create_block_visual(workspace, x, y, spec->width, spec->fill, spec->outline, 0xFFFFFF, spec->label);
    lv_obj_add_flag(block.object, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(block.object, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_ext_click_area(block.object, 5);
    lv_obj_add_event_cb(block.object, workspace_block_event, LV_EVENT_PRESSED, &block);
    return &block;
  }
  return nullptr;
}

void empty_space_pressed(lv_event_t*) { clear_selection(); }

void palette_pressed(lv_event_t*) {
  lv_point_t raw;
  if(!pointer_position(&raw)) return;

  lv_area_t paletteArea;
  lv_obj_get_coords(palette, &paletteArea);
  const int localX = raw.x - paletteArea.x1;
  const int localY = raw.y - paletteArea.y1;
  int matchedRow = -1;
  int selectedIndex = -1;

  if(localX >= 0 && localX < 180 && localY >= 0 && localY < 132) {
    matchedRow = localY / 44;
    selectedIndex = palettePage * kBlocksPerPage + matchedRow;
    if(selectedIndex < paletteSpecCount && paletteBlockStates[selectedIndex].object != nullptr) {
      select_palette_block(&paletteBlockStates[selectedIndex]);
    } else {
      selectedIndex = -1;
      clear_selection();
    }
  } else {
    clear_selection();
  }

  Serial.printf("BitBlocks palette press raw=(%d,%d) local=(%d,%d) row=%d block=%d\n",
                raw.x, raw.y, localX, localY, matchedRow, selectedIndex);
}

void add_palette_block(uint8_t index, int y, const char* label, const CategoryStyle& style) {
  (void)y;
  paletteSpecs[index] = {label, style.fill, style.outline, 160};
  if(index + 1 > paletteSpecCount) paletteSpecCount = index + 1;
}

uint8_t palette_page_count() {
  return paletteSpecCount == 0 ? 1 : (paletteSpecCount + kBlocksPerPage - 1) / kBlocksPerPage;
}

void render_palette_page();

void palette_page_arrow_pressed(lv_event_t* event) {
  const intptr_t direction = reinterpret_cast<intptr_t>(lv_event_get_user_data(event));
  const uint8_t pageCount = palette_page_count();
  if(direction < 0 && palettePage > 0) --palettePage;
  else if(direction > 0 && palettePage + 1 < pageCount) ++palettePage;
  else return;
  clear_selection();
  render_palette_page();
}

void create_palette_scrollwheel() {
  // Chunky, integer-aligned pixel scrollbar: dark outer rail, light inset,
  // and a discrete dark thumb.  Decorative pieces remain non-clickable.
  panel(palette, 175, 20, 14, 89, 0x111111, 0x111111, 0);
  panel(palette, 178, 23, 8, 83, 0xF2F2F2, 0xF2F2F2, 0);

  lv_obj_t* up = panel(palette, 175, 3, 14, 16, 0xF4F4F4, 0x111111, 2);
  lv_obj_add_flag(up, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_flag(up, LV_OBJ_FLAG_PRESS_LOCK);
  lv_obj_set_style_bg_color(up, color(0xD8D8D8), LV_STATE_PRESSED);
  lv_obj_add_event_cb(up, palette_page_arrow_pressed, LV_EVENT_PRESSED,
                      reinterpret_cast<void*>(static_cast<intptr_t>(-1)));
  panel(up, 6, 4, 2, 2, 0x111111, 0x111111, 0);
  panel(up, 4, 6, 6, 2, 0x111111, 0x111111, 0);
  panel(up, 2, 8, 10, 2, 0x111111, 0x111111, 0);

  lv_obj_t* down = panel(palette, 175, 110, 14, 16, 0xF4F4F4, 0x111111, 2);
  lv_obj_add_flag(down, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_flag(down, LV_OBJ_FLAG_PRESS_LOCK);
  lv_obj_set_style_bg_color(down, color(0xD8D8D8), LV_STATE_PRESSED);
  lv_obj_add_event_cb(down, palette_page_arrow_pressed, LV_EVENT_PRESSED,
                      reinterpret_cast<void*>(static_cast<intptr_t>(1)));
  panel(down, 2, 4, 10, 2, 0x111111, 0x111111, 0);
  panel(down, 4, 6, 6, 2, 0x111111, 0x111111, 0);
  panel(down, 6, 8, 2, 2, 0x111111, 0x111111, 0);

  const uint8_t pageCount = palette_page_count();
  const int thumbY = pageCount <= 1 ? 24 : 24 + (67 * palettePage) / (pageCount - 1);
  panel(palette, 178, thumbY, 8, 15, 0x171820, 0x171820, 0);
  panel(palette, 180, thumbY + 2, 4, 11, 0x343744, 0x343744, 0);
}

void render_palette_page() {
  lv_obj_clean(palette);
  for(PaletteBlockState& state : paletteBlockStates) state = PaletteBlockState{};

  const uint8_t first = palettePage * kBlocksPerPage;
  for(uint8_t slot = 0; slot < kBlocksPerPage; ++slot) {
    const uint8_t index = first + slot;
    if(index >= paletteSpecCount) break;
    const int y = 3 + slot * 44;
    const BlockSpec& spec = paletteSpecs[index];
    lv_obj_t* block = create_block_visual(palette, 2, y, spec.width, spec.fill, spec.outline, 0x242424, spec.label);
    PaletteBlockState& state = paletteBlockStates[index];
    state.object = block;
    state.artwork = block;
    state.spec = &paletteSpecs[index];
    state.id = index;
    state.hitRegion = {0, slot * 44, 179, slot * 44 + 43};
    lv_obj_clear_flag(block, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(block, LV_OBJ_FLAG_EVENT_BUBBLE);
  }

  if(paletteSpecCount == 0) text(palette, "NO CAMERA BLOCKS", 18, 55, &monogram_16, 0xD6D6D6);
  paletteInputLayer = lv_obj_create(palette);
  lv_obj_remove_style_all(paletteInputLayer);
  lv_obj_set_pos(paletteInputLayer, 0, 0);
  lv_obj_set_size(paletteInputLayer, 180, 132);
  lv_obj_set_style_bg_opa(paletteInputLayer, LV_OPA_TRANSP, 0);
  lv_obj_add_flag(paletteInputLayer, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_flag(paletteInputLayer, LV_OBJ_FLAG_PRESS_LOCK);
  lv_obj_clear_flag(paletteInputLayer, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(paletteInputLayer, palette_pressed, LV_EVENT_PRESSED, nullptr);
  create_palette_scrollwheel();
}

void close_category_dropdown() {
  if(categoryDropdown == nullptr) return;
  lv_obj_delete(categoryDropdown);
  categoryDropdown = nullptr;
}

void refresh_palette() {
  if(selectedPaletteBlock != nullptr) clear_selection();
  paletteSpecCount = 0;
  palettePage = 0;
  const CategoryStyle& style = categoryStyles[static_cast<uint8_t>(selectedCategory)];

  switch(selectedCategory) {
    case Category::Movement:
      add_palette_block(0, 6, "move [10] steps", style);
      add_palette_block(1, 48, "turn [15] degrees", style);
      add_palette_block(2, 90, "go x:[0] y:[0]", style);
      add_palette_block(3, 132, "set direction [90]", style);
      break;
    case Category::Events:
      add_palette_block(0, 6, "when [PLAY] clicked", style);
      add_palette_block(1, 48, "when [button] pressed", style);
      add_palette_block(2, 90, "when screen touched", style);
      add_palette_block(3, 132, "receive [message]", style);
      break;
    case Category::Control:
      add_palette_block(0, 6, "wait [0.5] seconds", style);
      add_palette_block(1, 48, "repeat [10] times", style);
      add_palette_block(2, 90, "forever", style);
      add_palette_block(3, 132, "if [condition] then", style);
      break;
    case Category::Operators:
      add_palette_block(0, 6, "[value] + [value]", style);
      add_palette_block(1, 48, "[value] - [value]", style);
      add_palette_block(2, 90, "[value] > [value]", style);
      add_palette_block(3, 132, "random [1] to [10]", style);
      break;
    case Category::Camera:
      break;
  }
  render_palette_page();
}

void update_category_header() {
  const CategoryStyle& style = categoryStyles[static_cast<uint8_t>(selectedCategory)];
  lv_obj_set_style_bg_color(categoryHeader, color(style.fill), 0);
  lv_obj_set_style_border_color(categoryHeader, color(style.outline), 0);
  lv_label_set_text_fmt(categoryLabel, "%s  %s", style.icon, style.name);
}

void category_option_clicked(lv_event_t* event) {
  selectedCategory = static_cast<Category>(reinterpret_cast<uintptr_t>(lv_event_get_user_data(event)));
  close_category_dropdown();
  update_category_header();
  refresh_palette();
}

void toggle_category_dropdown(lv_event_t*) {
  if(categoryDropdown != nullptr) {
    close_category_dropdown();
    return;
  }

  categoryDropdown = panel(root, 7, 44, 194, 160, 0x242424, 0x171820, 3);
  lv_obj_add_flag(categoryDropdown, LV_OBJ_FLAG_CLICKABLE);
  for(uint8_t i = 0; i < 5; ++i) {
    const CategoryStyle& style = categoryStyles[i];
    lv_obj_t* option = panel(categoryDropdown, 3, 3 + i * 31, 188, 30, style.fill, style.outline, 2);
    lv_obj_add_flag(option, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(option, category_option_clicked, LV_EVENT_CLICKED, reinterpret_cast<void*>(static_cast<uintptr_t>(i)));
    char optionText[28];
    lv_snprintf(optionText, sizeof(optionText), "%s  %s", style.icon, style.name);
    text(option, optionText, 8, 5, &monogram_16, 0xFFFFFF);
  }
  lv_obj_move_foreground(categoryDropdown);
}
}

void BitBlocksApp::create() {
  for(WorkspaceBlock& block : workspaceBlocks) block = WorkspaceBlock{};
  selectedBlock = nullptr;
  selectedPaletteBlock = nullptr;
  root = lv_obj_create(nullptr);base(root, 0xD9DBE3);lv_obj_set_size(root, 480, 320);lv_screen_load(root);

  lv_obj_t* sidebar = panel(root, 4, 4, 200, 312, 0xD9DBE3, 0x34302D, 0);
  categoryHeader = panel(sidebar, 3, 3, 194, 37, 0x4D69B2, 0x29365F, 3);
  lv_obj_add_flag(categoryHeader, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(categoryHeader, toggle_category_dropdown, LV_EVENT_CLICKED, nullptr);
  categoryLabel = text(categoryHeader, "", 10, 8, &monogram_20, 0xFFFFFF);
  text(categoryHeader, "v", 175, 8, &monogram_20, 0xFFFFFF);
  palette = panel(sidebar, 3, 44, 194, 133, 0x242424, 0x382D27, 3);
  lv_obj_t* preview = panel(sidebar, 3, 181, 192, 128, 0xFAFAFA, 0x29231F, 3);
  text(preview, "wilderness + capybara", 26, 51, &monogram_16, 0x30384B);
  update_category_header();
  refresh_palette();

  workspace = panel(root, 210, 4, 266, 312, 0xFFFFFF, 0x34302D, 2);
  lv_obj_add_flag(workspace, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(workspace, empty_space_pressed, LV_EVENT_PRESSED, nullptr);
  playButton = panel(workspace, 194, 266, 28, 36, 0x4B843D, 0x171820, 3);
  text(playButton, ">", 9, 7, &monogram_24, 0xFFFFFF);
  stopButton = panel(workspace, 228, 266, 28, 36, 0xDF244B, 0x171820, 3);
  text(stopButton, "[]", 6, 8, &monogram_20, 0xFFFFFF);
}

void BitBlocksApp::destroy() {
  root = nullptr;
  categoryHeader = nullptr;
  categoryLabel = nullptr;
  palette = nullptr;
  categoryDropdown = nullptr;
  workspace = nullptr;
  playButton = nullptr;
  stopButton = nullptr;
  selectedBlock = nullptr;
  selectedPaletteBlock = nullptr;
}

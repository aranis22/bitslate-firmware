#include "bitblocks_app.h"
#include "block_model.h"
#include "bitblocks_ui_assets.h"
#include "../../assets/UI/bitblocks/generated/sprites_64/sprites_64_manifest.h"
#include "../../assets/UI/bitblocks/generated/backdrops_480x320/backdrops_480x320_manifest.h"

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
lv_obj_t* trashControl = nullptr;
lv_obj_t* previewPanel = nullptr;
lv_obj_t* previewBackdrop = nullptr;
lv_obj_t* previewSprite = nullptr;
lv_obj_t* spriteToolbar = nullptr;
lv_obj_t* backdropToolbar = nullptr;
lv_obj_t* toolbarButtons[2][3] = {};
lv_obj_t* modalPage = nullptr;

struct MediaAsset { const char* name; const lv_image_dsc_t* image; };
constexpr MediaAsset sprites[] = {
  {"astronaut",&bitblocks_sprite_64_astronaut},{"capybara",&bitblocks_sprite_64_capybara},
  {"car",&bitblocks_sprite_64_car},{"cat",&bitblocks_sprite_64_cat},{"dog",&bitblocks_sprite_64_dog},
  {"fish",&bitblocks_sprite_64_fish},{"horse",&bitblocks_sprite_64_horse},{"knight",&bitblocks_sprite_64_knight},
  {"santa",&bitblocks_sprite_64_santa},{"snowman",&bitblocks_sprite_64_snowman},{"ufo",&bitblocks_sprite_64_ufo},
  {"wizard",&bitblocks_sprite_64_wizard},
};
constexpr MediaAsset backdrops[] = {
  {"city",&bitblocks_backdrop_480x320_city},{"city2",&bitblocks_backdrop_480x320_city2},
  {"room",&bitblocks_backdrop_480x320_room},{"space",&bitblocks_backdrop_480x320_space},
  {"space2",&bitblocks_backdrop_480x320_space2},{"space3",&bitblocks_backdrop_480x320_space3},
  {"underwater",&bitblocks_backdrop_480x320_underwater},{"wilderness",&bitblocks_backdrop_480x320_wilderness},
  {"wilderness2",&bitblocks_backdrop_480x320_wilderness2},{"wilderness3",&bitblocks_backdrop_480x320_wilderness3},
  {"woods",&bitblocks_backdrop_480x320_woods},
};
constexpr uint8_t kSpriteCount=sizeof(sprites)/sizeof(sprites[0]);
constexpr uint8_t kBackdropCount=sizeof(backdrops)/sizeof(backdrops[0]);
uint8_t currentSprite = 1;
uint8_t currentBackdrop = 7;
bool spriteToolbarExpanded = false;
bool backdropToolbarExpanded = false;

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
  lv_obj_t* notchTop = nullptr;
  lv_obj_t* notchInner = nullptr;
  BlockSpec ownedSpec = {};
  const BlockSpec* spec = nullptr;
  bitblocks::BlockId id = bitblocks::kInvalidBlockId;
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
bitblocks::WorkspaceModel workspaceModel;
WorkspaceBlock* selectedBlock = nullptr;
PaletteBlockState paletteBlockStates[4];
PaletteBlockState* selectedPaletteBlock = nullptr;

BlockSpec paletteSpecs[4];
uint8_t paletteSpecCount = 0;
uint8_t palettePage = 0;

constexpr uint8_t kBlocksPerPage = 3;
constexpr int kWorkspaceX = 210;
constexpr int kWorkspaceY = 4;
constexpr int kWorkspaceWidth = 266;
constexpr int kWorkspaceHeight = 312;
constexpr int kStackStep = 31;
constexpr int kDragThreshold = 6;
constexpr uint32_t kHoldDelayMs = 120;
constexpr int kSnapXThreshold = 24;
constexpr int kSnapYThreshold = 12;

enum class InteractionState : uint8_t {
  Idle,
  PressedPalette,
  DraggingPaletteClone,
  PressedWorkspace,
  DraggingWorkspaceBlock,
};

struct Interaction {
  InteractionState state = InteractionState::Idle;
  lv_point_t pressPoint = {};
  lv_point_t lastPoint = {};
  uint32_t pressTick = 0;
  int paletteIndex = -1;
  bitblocks::BlockId blockId = bitblocks::kInvalidBlockId;
  lv_obj_t* floating = nullptr;
  int offsetX = 0;
  int offsetY = 0;
};

Interaction interaction;

lv_color_t color(uint32_t hex) { return lv_color_hex(hex); }

void base(lv_obj_t* obj, uint32_t fill, uint32_t border = 0, int borderWidth = 0) {
  lv_obj_remove_style_all(obj);
  lv_obj_set_style_bg_color(obj, color(fill), 0);
  lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(obj, color(border), 0);
  lv_obj_set_style_border_width(obj, borderWidth, 0);
  lv_obj_set_style_radius(obj, 0, 0);
  lv_obj_set_style_pad_all(obj, 0, 0);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
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

lv_obj_t* image_button(lv_obj_t* parent, int x, int y, const lv_image_dsc_t* source,
                       lv_event_cb_t callback = nullptr, void* userData = nullptr) {
  lv_obj_t* button = panel(parent, x, y, 30, 30, 0xFFFFFF, 0xFFFFFF, 0);
  lv_obj_set_style_bg_opa(button, LV_OPA_TRANSP, 0);
  lv_obj_t* image = lv_image_create(button);lv_image_set_src(image, source);lv_obj_set_pos(image, -1, -1);
  lv_obj_clear_flag(image, LV_OBJ_FLAG_CLICKABLE);
  if(callback != nullptr) {
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);lv_obj_add_flag(button, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_style_opa(button, LV_OPA_60, LV_STATE_PRESSED);
    lv_obj_add_event_cb(button, callback, LV_EVENT_CLICKED, userData);
  }
  return button;
}

void update_preview_media() {
  if(previewBackdrop != nullptr) lv_image_set_src(previewBackdrop, backdrops[currentBackdrop].image);
  if(previewSprite != nullptr) lv_image_set_src(previewSprite, sprites[currentSprite].image);
}

enum class ModalKind : uint8_t { Sprite, Backdrop, Fullscreen };
ModalKind modalKind = ModalKind::Sprite;
uint8_t modalPageIndex = 0;
uint8_t modalSelection = 0;
lv_obj_t* modalCards[4] = {};

void close_modal(lv_event_t*) {
  if(modalPage == nullptr) return;
  lv_obj_delete(modalPage);modalPage = nullptr;
}

void render_selector_cards();

void selector_card_clicked(lv_event_t* event) {
  const uint8_t slot = static_cast<uint8_t>(reinterpret_cast<uintptr_t>(lv_event_get_user_data(event)));
  modalSelection = modalPageIndex * 4 + slot;render_selector_cards();
}

void selector_page_change(lv_event_t* event) {
  const intptr_t direction = reinterpret_cast<intptr_t>(lv_event_get_user_data(event));
  const uint8_t count = modalKind == ModalKind::Sprite ? kSpriteCount : kBackdropCount;
  const uint8_t pages = (count + 3) / 4;
  if(direction < 0 && modalPageIndex > 0) --modalPageIndex;
  else if(direction > 0 && modalPageIndex + 1 < pages) ++modalPageIndex;
  else return;
  render_selector_cards();
}

void confirm_selector(lv_event_t*) {
  if(modalKind == ModalKind::Sprite) currentSprite = modalSelection;
  else currentBackdrop = modalSelection;
  update_preview_media();close_modal(nullptr);
}

void render_selector_cards() {
  if(modalPage == nullptr) return;
  for(lv_obj_t*& card : modalCards) {if(card != nullptr) lv_obj_delete(card);card = nullptr;}
  const MediaAsset* assets = modalKind == ModalKind::Sprite ? sprites : backdrops;
  const uint8_t count = modalKind == ModalKind::Sprite ? kSpriteCount : kBackdropCount;
  for(uint8_t slot=0;slot<4;++slot) {
    const uint8_t index=modalPageIndex*4+slot;if(index>=count) break;
    const int x=8+slot*116;lv_obj_t* card=panel(modalPage,x,62,108,178,0xF4F1F6,index==modalSelection?0xFFE45C:0xB7B1C0,index==modalSelection?4:3);
    lv_obj_add_flag(card,LV_OBJ_FLAG_CLICKABLE);lv_obj_add_event_cb(card,selector_card_clicked,LV_EVENT_CLICKED,reinterpret_cast<void*>(static_cast<uintptr_t>(slot)));
    lv_obj_t* image=lv_image_create(card);lv_image_set_src(image,assets[index].image);lv_obj_clear_flag(image,LV_OBJ_FLAG_CLICKABLE);
    if(modalKind==ModalKind::Sprite) lv_obj_set_pos(image,22,28);
    else {lv_image_set_scale(image,51);lv_obj_set_pos(image,(108-480)/2,12+(82-320)/2);}
    lv_obj_t* label=text(card,assets[index].name,5,143,&monogram_16,0x24202B);lv_obj_set_width(label,98);lv_obj_set_style_text_align(label,LV_TEXT_ALIGN_CENTER,0);
    modalCards[slot]=card;
  }
}

void open_selector(ModalKind kind) {
  if(modalPage != nullptr) return;
  modalKind=kind;modalSelection=kind==ModalKind::Sprite?currentSprite:currentBackdrop;modalPageIndex=modalSelection/4;
  modalPage=panel(root,0,0,480,320,kind==ModalKind::Sprite?0x342057:0x62656B,0,0);lv_obj_move_foreground(modalPage);
  text(modalPage,kind==ModalKind::Sprite?"SPRITE SELECTOR":"BACKDROP SELECTOR",16,12,&monogram_24,0xFFFFFF);
  lv_obj_t* back=panel(modalPage,10,270,74,36,0x30384B,0xF2EDF4,3);lv_obj_add_flag(back,LV_OBJ_FLAG_CLICKABLE);lv_obj_add_event_cb(back,close_modal,LV_EVENT_CLICKED,nullptr);text(back,"< BACK",10,7,&monogram_16,0xFFFFFF);
  lv_obj_t* confirm=panel(modalPage,374,270,96,36,0x4B843D,0x18271B,3);lv_obj_add_flag(confirm,LV_OBJ_FLAG_CLICKABLE);lv_obj_add_event_cb(confirm,confirm_selector,LV_EVENT_CLICKED,nullptr);text(confirm,"CONFIRM",13,7,&monogram_16,0xFFFFFF);
  lv_obj_t* previous=panel(modalPage,175,270,44,36,0xEDEAF0,0x171820,3);lv_obj_add_flag(previous,LV_OBJ_FLAG_CLICKABLE);lv_obj_add_event_cb(previous,selector_page_change,LV_EVENT_CLICKED,reinterpret_cast<void*>(static_cast<intptr_t>(-1)));text(previous,"<",16,6,&monogram_20,0x171820);
  lv_obj_t* next=panel(modalPage,261,270,44,36,0xEDEAF0,0x171820,3);lv_obj_add_flag(next,LV_OBJ_FLAG_CLICKABLE);lv_obj_add_event_cb(next,selector_page_change,LV_EVENT_CLICKED,reinterpret_cast<void*>(static_cast<intptr_t>(1)));text(next,">",16,6,&monogram_20,0x171820);
  render_selector_cards();
}

void open_selector_event(lv_event_t* event) {open_selector(static_cast<ModalKind>(reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))));}

void randomize_media(lv_event_t* event) {
  const ModalKind kind=static_cast<ModalKind>(reinterpret_cast<uintptr_t>(lv_event_get_user_data(event)));
  if(kind==ModalKind::Sprite) currentSprite=esp_random()%kSpriteCount;
  else currentBackdrop=esp_random()%kBackdropCount;
  update_preview_media();
}

void toggle_media_toolbar(lv_event_t* event) {
  const uint8_t toolbarIndex=static_cast<uint8_t>(reinterpret_cast<uintptr_t>(lv_event_get_user_data(event)));
  bool& expanded=toolbarIndex==0?spriteToolbarExpanded:backdropToolbarExpanded;expanded=!expanded;
  for(lv_obj_t* button:toolbarButtons[toolbarIndex]) if(button!=nullptr) lv_obj_set_flag(button,LV_OBJ_FLAG_HIDDEN,!expanded);
}

lv_obj_t* create_media_toolbar(lv_obj_t* parent,int x,ModalKind kind,const lv_image_dsc_t* triggerSource) {
  const uint8_t index=kind==ModalKind::Sprite?0:1;
  lv_obj_t* toolbar=panel(parent,x,4,30,120,0xFFFFFF,0xFFFFFF,0);lv_obj_set_style_bg_opa(toolbar,LV_OPA_TRANSP,0);
  toolbarButtons[index][0]=image_button(toolbar,0,0,&bitblocks_sparkle_icon,randomize_media,reinterpret_cast<void*>(static_cast<uintptr_t>(kind)));
  toolbarButtons[index][1]=image_button(toolbar,0,30,&bitblocks_paint_icon);
  toolbarButtons[index][2]=image_button(toolbar,0,60,&bitblocks_search_icon,open_selector_event,reinterpret_cast<void*>(static_cast<uintptr_t>(kind)));
  for(lv_obj_t* button:toolbarButtons[index]) lv_obj_add_flag(button,LV_OBJ_FLAG_HIDDEN);
  image_button(toolbar,0,90,triggerSource,toggle_media_toolbar,reinterpret_cast<void*>(static_cast<uintptr_t>(index)));
  return toolbar;
}

void open_fullscreen(lv_event_t*) {
  if(modalPage != nullptr) return;modalKind=ModalKind::Fullscreen;modalPage=panel(root,0,0,480,320,0x000000,0,0);lv_obj_move_foreground(modalPage);
  lv_obj_t* backdrop=lv_image_create(modalPage);lv_image_set_src(backdrop,backdrops[currentBackdrop].image);lv_obj_set_pos(backdrop,0,0);
  lv_obj_t* sprite=lv_image_create(modalPage);lv_image_set_src(sprite,sprites[currentSprite].image);lv_obj_set_pos(sprite,208,128);
  lv_obj_t* back=panel(modalPage,10,10,76,38,0x30384B,0xFFFFFF,3);lv_obj_add_flag(back,LV_OBJ_FLAG_CLICKABLE);lv_obj_add_event_cb(back,close_modal,LV_EVENT_CLICKED,nullptr);text(back,"< BACK",10,8,&monogram_16,0xFFFFFF);
}

lv_obj_t* create_block_visual(lv_obj_t* parent, int x, int y, int w, uint32_t fill, uint32_t outline,
                              uint32_t surrounding, const char* label, lv_obj_t** notchTop = nullptr,
                              lv_obj_t** notchInner = nullptr) {
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
  lv_obj_t* topFill = panel(block, 20, 0, 25, 6, surrounding, surrounding, 0);
  panel(block, 20, 4, 4, 4, outline, outline, 0);
  panel(block, 41, 4, 4, 4, outline, outline, 0);
  panel(block, 24, 5, 17, 3, outline, outline, 0);
  lv_obj_t* innerFill = panel(block, 25, 5, 15, 2, surrounding, surrounding, 0);
  if(notchTop != nullptr) *notchTop = topFill;
  if(notchInner != nullptr) *notchInner = innerFill;
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

WorkspaceBlock* view_for(bitblocks::BlockId id) {
  if(id < 0 || id >= kMaxWorkspaceBlocks || !workspaceBlocks[id].active) return nullptr;
  return &workspaceBlocks[id];
}

void set_notch_color(WorkspaceBlock* view, uint32_t fill) {
  if(view == nullptr) return;
  for(lv_obj_t* part : {view->notchTop, view->notchInner}) {
    if(part == nullptr) continue;
    lv_obj_set_style_bg_color(part, color(fill), 0);
    lv_obj_set_style_border_color(part, color(fill), 0);
  }
}

void refresh_workspace_notch(bitblocks::BlockId id) {
  WorkspaceBlock* view = view_for(id);
  const bitblocks::BlockModel* model = workspaceModel.get(id);
  if(view == nullptr || model == nullptr) return;
  uint32_t notchFill = 0xFFFFFF;
  if(model->previous != bitblocks::kInvalidBlockId) {
    WorkspaceBlock* previous = view_for(model->previous);
    if(previous != nullptr && previous->spec != nullptr) notchFill = previous->spec->fill;
  }
  set_notch_color(view, notchFill);
}

void sync_chain_views(bitblocks::BlockId rootId) {
  for(bitblocks::BlockId id = rootId; id != bitblocks::kInvalidBlockId;) {
    const bitblocks::BlockModel* model = workspaceModel.get(id);
    WorkspaceBlock* view = view_for(id);
    if(model == nullptr || view == nullptr) break;
    lv_obj_set_pos(view->object, model->x, model->y);
    lv_obj_move_foreground(view->object);
    id = model->next;
  }
  if(playButton != nullptr) lv_obj_move_foreground(playButton);
  if(stopButton != nullptr) lv_obj_move_foreground(stopButton);
  if(trashControl != nullptr) lv_obj_move_foreground(trashControl);
}

void clear_selection() {
  for(WorkspaceBlock& block : workspaceBlocks) {
    if(block.selection == nullptr) continue;
    lv_obj_delete(block.selection);
    block.selection = nullptr;
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
  for(bitblocks::BlockId id = block->id; id != bitblocks::kInvalidBlockId;) {
    WorkspaceBlock* view = view_for(id);
    const bitblocks::BlockModel* model = workspaceModel.get(id);
    if(view == nullptr || model == nullptr) break;
    view->selection = create_selection_overlay(view->object, view->spec->width);
    id = model->next;
  }
}

void select_palette_block(PaletteBlockState* block) {
  if(selectedPaletteBlock == block && selectedBlock == nullptr) return;
  clear_selection();
  selectedPaletteBlock = block;
  block->selection = create_selection_overlay(block->artwork, block->spec->width);
}

WorkspaceBlock* allocate_workspace_block(const BlockSpec* spec, int x, int y);

void snap_stack(bitblocks::BlockId draggedId) {
  bitblocks::BlockModel* dragged = workspaceModel.get(draggedId);
  if(dragged == nullptr) return;
  for(int id = 0; id < kMaxWorkspaceBlocks; ++id) {
    bitblocks::BlockModel* candidate = workspaceModel.get(static_cast<bitblocks::BlockId>(id));
    if(candidate == nullptr || workspaceModel.chainContains(draggedId, candidate->id)) continue;
    if(candidate->next == bitblocks::kInvalidBlockId &&
       LV_ABS(dragged->x - candidate->x) <= kSnapXThreshold &&
       LV_ABS(dragged->y - (candidate->y + kStackStep)) <= kSnapYThreshold) {
      const int dx = candidate->x - dragged->x;
      const int dy = candidate->y + kStackStep - dragged->y;
      if(workspaceModel.connect(candidate->id, draggedId)) {
        workspaceModel.moveChain(draggedId, dx, dy);
        refresh_workspace_notch(draggedId);
        sync_chain_views(draggedId);
      }
      return;
    }
  }

  const bitblocks::BlockId tailId = workspaceModel.tail(draggedId);
  bitblocks::BlockModel* tail = workspaceModel.get(tailId);
  if(tail == nullptr) return;
  for(int id = 0; id < kMaxWorkspaceBlocks; ++id) {
    bitblocks::BlockModel* candidate = workspaceModel.get(static_cast<bitblocks::BlockId>(id));
    if(candidate == nullptr || candidate->previous != bitblocks::kInvalidBlockId ||
       workspaceModel.chainContains(draggedId, candidate->id)) continue;
    if(LV_ABS(tail->x - candidate->x) <= kSnapXThreshold &&
       LV_ABS(candidate->y - (tail->y + kStackStep)) <= kSnapYThreshold) {
      if(workspaceModel.connect(tailId, candidate->id)) {
        workspaceModel.moveChain(candidate->id, tail->x - candidate->x,
                                  tail->y + kStackStep - candidate->y);
        refresh_workspace_notch(candidate->id);
        sync_chain_views(candidate->id);
      }
      return;
    }
  }
}

bool moved_beyond_threshold(const lv_point_t& point) {
  return LV_ABS(point.x - interaction.pressPoint.x) >= kDragThreshold ||
         LV_ABS(point.y - interaction.pressPoint.y) >= kDragThreshold;
}

bool inside_workspace(const lv_point_t& point) {
  lv_area_t area;
  lv_obj_get_coords(workspace, &area);
  return point.x >= area.x1 && point.x <= area.x2 && point.y >= area.y1 && point.y <= area.y2;
}

bool inside_trash(const lv_point_t& point) {
  if(trashControl == nullptr) return false;
  lv_area_t area;
  lv_obj_get_coords(trashControl, &area);
  return point.x >= area.x1 && point.x <= area.x2 && point.y >= area.y1 && point.y <= area.y2;
}

void set_trash_active(bool active) {
  if(trashControl == nullptr) return;
  lv_obj_set_style_outline_width(trashControl, active ? 3 : 0, 0);
  lv_obj_set_style_outline_color(trashControl, color(0xFFE45C), 0);
  lv_obj_set_style_outline_opa(trashControl, active ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
}

void delete_workspace_chain(bitblocks::BlockId rootId) {
  bitblocks::BlockId ids[kMaxWorkspaceBlocks];
  uint8_t count = 0;
  for(bitblocks::BlockId id = rootId; id != bitblocks::kInvalidBlockId && count < kMaxWorkspaceBlocks;) {
    ids[count++] = id;
    const bitblocks::BlockModel* model = workspaceModel.get(id);
    if(model == nullptr) break;
    id = model->next;
  }
  clear_selection();
  workspaceModel.removeChain(rootId);
  for(uint8_t i = 0; i < count; ++i) {
    WorkspaceBlock* view = view_for(ids[i]);
    if(view == nullptr) continue;
    if(view->object != nullptr) lv_obj_delete_async(view->object);
    *view = WorkspaceBlock{};
  }
  set_trash_active(false);
}

void workspace_local(const lv_point_t& point, int* x, int* y) {
  lv_area_t area;
  lv_obj_get_coords(workspace, &area);
  *x = point.x - area.x1;
  *y = point.y - area.y1;
}

void reset_interaction() {
  if(interaction.floating != nullptr) lv_obj_delete(interaction.floating);
  interaction = Interaction{};
}

void update_palette_drag(const lv_point_t& point) {
  if(interaction.paletteIndex < 0 || interaction.paletteIndex >= paletteSpecCount) return;
  const BlockSpec& spec = paletteSpecs[interaction.paletteIndex];
  if(interaction.state == InteractionState::PressedPalette) {
    if(lv_tick_elaps(interaction.pressTick) < kHoldDelayMs || !moved_beyond_threshold(point)) return;
    interaction.state = InteractionState::DraggingPaletteClone;
    interaction.offsetX = spec.width / 2;
    interaction.offsetY = 18;
    interaction.floating = create_block_visual(root, point.x - interaction.offsetX, point.y - interaction.offsetY,
                                                spec.width, spec.fill, spec.outline, 0xD9DBE3, spec.label);
    lv_obj_clear_flag(interaction.floating, LV_OBJ_FLAG_CLICKABLE);
  }
  if(interaction.state == InteractionState::DraggingPaletteClone && interaction.floating != nullptr) {
    lv_obj_set_pos(interaction.floating, point.x - interaction.offsetX, point.y - interaction.offsetY);
    lv_obj_move_foreground(interaction.floating);
  }
}

void update_workspace_drag(const lv_point_t& point) {
  bitblocks::BlockModel* model = workspaceModel.get(interaction.blockId);
  WorkspaceBlock* view = view_for(interaction.blockId);
  if(model == nullptr || view == nullptr) return;
  if(interaction.state == InteractionState::PressedWorkspace) {
    if(lv_tick_elaps(interaction.pressTick) < kHoldDelayMs || !moved_beyond_threshold(point)) return;
    workspaceModel.detachPrevious(interaction.blockId);
    refresh_workspace_notch(interaction.blockId);
    interaction.state = InteractionState::DraggingWorkspaceBlock;
  }
  if(interaction.state != InteractionState::DraggingWorkspaceBlock) return;
  int localX, localY;
  workspace_local(point, &localX, &localY);
  int targetX = localX - interaction.offsetX;
  int targetY = localY - interaction.offsetY;
  targetX = LV_CLAMP(2, targetX, kWorkspaceWidth - view->spec->width - 2);
  int chainHeight = 37;
  const bitblocks::BlockId tailId = workspaceModel.tail(interaction.blockId);
  const bitblocks::BlockModel* tail = workspaceModel.get(tailId);
  if(tail != nullptr) chainHeight += tail->y - model->y;
  targetY = LV_CLAMP(2, targetY, kWorkspaceHeight - chainHeight - 2);
  workspaceModel.moveChain(interaction.blockId, targetX - model->x, targetY - model->y);
  sync_chain_views(interaction.blockId);
  set_trash_active(inside_trash(point));
  interaction.lastPoint = point;
}

void workspace_block_event(lv_event_t* event) {
  WorkspaceBlock* block = static_cast<WorkspaceBlock*>(lv_event_get_user_data(event));
  const lv_event_code_t code = lv_event_get_code(event);
  lv_point_t point;
  if(!pointer_position(&point)) return;
  if(code == LV_EVENT_PRESSED) {
    select_block(block);
    interaction = Interaction{};
    interaction.state = InteractionState::PressedWorkspace;
    interaction.blockId = block->id;
    interaction.pressPoint = point;
    interaction.lastPoint = point;
    interaction.pressTick = lv_tick_get();
    interaction.offsetX = point.x - (kWorkspaceX + lv_obj_get_x(block->object));
    interaction.offsetY = point.y - (kWorkspaceY + lv_obj_get_y(block->object));
  } else if(code == LV_EVENT_PRESSING) {
    update_workspace_drag(point);
  } else if(code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
    if(interaction.state == InteractionState::DraggingWorkspaceBlock) {
      if(inside_trash(point)) delete_workspace_chain(interaction.blockId);
      else snap_stack(interaction.blockId);
    }
    set_trash_active(false);
    interaction = Interaction{};
  }
}

WorkspaceBlock* allocate_workspace_block(const BlockSpec* spec, int x, int y) {
  const bitblocks::BlockId id = workspaceModel.create(x, y);
  if(id == bitblocks::kInvalidBlockId) return nullptr;
  WorkspaceBlock& block = workspaceBlocks[id];
    block = WorkspaceBlock{};
    block.active = true;
    block.id = id;
    block.ownedSpec = *spec;
    block.spec = &block.ownedSpec;
    block.object = create_block_visual(workspace, x, y, spec->width, spec->fill, spec->outline, 0xFFFFFF,
                                       spec->label, &block.notchTop, &block.notchInner);
    lv_obj_add_flag(block.object, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(block.object, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_ext_click_area(block.object, 5);
    lv_obj_add_event_cb(block.object, workspace_block_event, LV_EVENT_ALL, &block);
    return &block;
}

void empty_space_pressed(lv_event_t* event) {
  if(lv_event_get_target(event) == workspace) clear_selection();
}

void palette_pressed(lv_event_t* event) {
  const lv_event_code_t code = lv_event_get_code(event);
  lv_point_t raw;
  if(!pointer_position(&raw)) return;

  if(code == LV_EVENT_PRESSING) {
    update_palette_drag(raw);
    return;
  }
  if(code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
    if(interaction.state == InteractionState::DraggingPaletteClone && inside_trash(raw)) {
      reset_interaction();
      return;
    }
    if(interaction.state == InteractionState::DraggingPaletteClone && inside_workspace(raw) &&
       interaction.paletteIndex >= 0 && interaction.paletteIndex < paletteSpecCount) {
      int localX, localY;
      workspace_local(raw, &localX, &localY);
      const BlockSpec& spec = paletteSpecs[interaction.paletteIndex];
      localX = LV_CLAMP(2, localX - interaction.offsetX, kWorkspaceWidth - spec.width - 2);
      localY = LV_CLAMP(2, localY - interaction.offsetY, kWorkspaceHeight - 39);
      WorkspaceBlock* placed = allocate_workspace_block(&spec, localX, localY);
      if(placed != nullptr) {
        select_block(placed);
        snap_stack(placed->id);
      }
    }
    reset_interaction();
    return;
  }
  if(code != LV_EVENT_PRESSED) return;

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
      interaction = Interaction{};
      interaction.state = InteractionState::PressedPalette;
      interaction.paletteIndex = selectedIndex;
      interaction.pressPoint = raw;
      interaction.lastPoint = raw;
      interaction.pressTick = lv_tick_get();
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
  lv_obj_add_event_cb(paletteInputLayer, palette_pressed, LV_EVENT_ALL, nullptr);
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
  workspaceModel.reset();
  interaction = Interaction{};
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
  previewPanel = panel(sidebar, 3, 181, 192, 128, 0xFAFAFA, 0x29231F, 3);
  previewBackdrop=lv_image_create(previewPanel);lv_image_set_src(previewBackdrop,backdrops[currentBackdrop].image);lv_image_set_scale(previewBackdrop,102);lv_obj_set_pos(previewBackdrop,-144,-96);lv_obj_clear_flag(previewBackdrop,LV_OBJ_FLAG_CLICKABLE);
  previewSprite=lv_image_create(previewPanel);lv_image_set_src(previewSprite,sprites[currentSprite].image);lv_obj_set_pos(previewSprite,64,32);lv_obj_clear_flag(previewSprite,LV_OBJ_FLAG_CLICKABLE);
  image_button(previewPanel,4,94,&bitblocks_view_icon,open_fullscreen,nullptr);
  spriteToolbar=create_media_toolbar(previewPanel,128,ModalKind::Sprite,&bitblocks_capybara_plus_icon);
  backdropToolbar=create_media_toolbar(previewPanel,160,ModalKind::Backdrop,&bitblocks_image_icon);
  update_category_header();
  refresh_palette();

  workspace = panel(root, kWorkspaceX, kWorkspaceY, kWorkspaceWidth, kWorkspaceHeight, 0xFFFFFF, 0x34302D, 2);
  lv_obj_add_flag(workspace, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(workspace, empty_space_pressed, LV_EVENT_PRESSED, nullptr);
  auto imageControl = [](int x, const lv_image_dsc_t* source) {
    lv_obj_t* control = panel(workspace, x, 271, 34, 34, 0xFFFFFF, 0xFFFFFF, 0);
    lv_obj_set_style_bg_opa(control, LV_OPA_TRANSP, 0);
    lv_obj_t* image = lv_image_create(control);
    lv_image_set_src(image, source);
    lv_obj_set_pos(image, 1, 1);
    lv_obj_clear_flag(image, LV_OBJ_FLAG_CLICKABLE);
    return control;
  };
  trashControl = imageControl(6, &bitblocks_trash_icon);
  playButton = imageControl(193, &bitblocks_play_icon);
  stopButton = imageControl(229, &bitblocks_stop_icon);
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
  trashControl = nullptr;
  previewPanel = nullptr;previewBackdrop = nullptr;previewSprite = nullptr;
  spriteToolbar = nullptr;backdropToolbar = nullptr;modalPage = nullptr;
  selectedBlock = nullptr;
  selectedPaletteBlock = nullptr;
}

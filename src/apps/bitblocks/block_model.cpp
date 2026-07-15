#include "block_model.h"

namespace bitblocks {

void WorkspaceModel::reset() {
  for(uint8_t i = 0; i < kMaxBlocks; ++i) blocks_[i] = BlockModel{};
}

BlockId WorkspaceModel::create(int16_t x, int16_t y) {
  for(uint8_t i = 0; i < kMaxBlocks; ++i) {
    if(blocks_[i].active) continue;
    blocks_[i] = BlockModel{};
    blocks_[i].active = true;
    blocks_[i].id = static_cast<BlockId>(i);
    blocks_[i].x = x;
    blocks_[i].y = y;
    return blocks_[i].id;
  }
  return kInvalidBlockId;
}

BlockModel* WorkspaceModel::get(BlockId id) {
  if(id < 0 || id >= static_cast<BlockId>(kMaxBlocks) || !blocks_[id].active) return nullptr;
  return &blocks_[id];
}

const BlockModel* WorkspaceModel::get(BlockId id) const {
  if(id < 0 || id >= static_cast<BlockId>(kMaxBlocks) || !blocks_[id].active) return nullptr;
  return &blocks_[id];
}

void WorkspaceModel::detachPrevious(BlockId id) {
  BlockModel* block = get(id);
  if(block == nullptr || block->previous == kInvalidBlockId) return;
  BlockModel* previous = get(block->previous);
  if(previous != nullptr && previous->next == id) previous->next = kInvalidBlockId;
  block->previous = kInvalidBlockId;
}

bool WorkspaceModel::connect(BlockId previousId, BlockId nextId) {
  BlockModel* previous = get(previousId);
  BlockModel* next = get(nextId);
  if(previous == nullptr || next == nullptr || previousId == nextId) return false;
  if(previous->next != kInvalidBlockId || next->previous != kInvalidBlockId) return false;
  if(chainContains(nextId, previousId)) return false;
  previous->next = nextId;
  next->previous = previousId;
  return true;
}

bool WorkspaceModel::chainContains(BlockId root, BlockId candidate) const {
  for(BlockId id = root; id != kInvalidBlockId;) {
    if(id == candidate) return true;
    const BlockModel* block = get(id);
    if(block == nullptr) break;
    id = block->next;
  }
  return false;
}

BlockId WorkspaceModel::tail(BlockId root) const {
  BlockId result = root;
  for(const BlockModel* block = get(result); block != nullptr && block->next != kInvalidBlockId;) {
    result = block->next;
    block = get(result);
  }
  return result;
}

void WorkspaceModel::moveChain(BlockId root, int16_t dx, int16_t dy) {
  for(BlockId id = root; id != kInvalidBlockId;) {
    BlockModel* block = get(id);
    if(block == nullptr) break;
    block->x += dx;
    block->y += dy;
    id = block->next;
  }
}

}  // namespace bitblocks

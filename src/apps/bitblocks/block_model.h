#pragma once

#include <cstdint>

namespace bitblocks {

using BlockId = int8_t;
constexpr BlockId kInvalidBlockId = -1;
constexpr uint8_t kMaxBlocks = 24;

struct BlockModel {
  bool active = false;
  BlockId id = kInvalidBlockId;
  BlockId previous = kInvalidBlockId;
  BlockId next = kInvalidBlockId;
  BlockId parent = kInvalidBlockId;
  int16_t x = 0;
  int16_t y = 0;
};

class WorkspaceModel {
 public:
  void reset();
  BlockId create(int16_t x, int16_t y);
  BlockModel* get(BlockId id);
  const BlockModel* get(BlockId id) const;
  void detachPrevious(BlockId id);
  bool connect(BlockId previous, BlockId next);
  bool chainContains(BlockId root, BlockId candidate) const;
  BlockId tail(BlockId root) const;
  void moveChain(BlockId root, int16_t dx, int16_t dy);
  void removeChain(BlockId root);

 private:
  BlockModel blocks_[kMaxBlocks] = {};
};

}  // namespace bitblocks

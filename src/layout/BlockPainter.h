#pragma once
#include "layout/DisplayItem.h"
#include <memory>
#include <vector>

class BlockLayout;

namespace layout {

/**
 * Story: Responsible for the painting phase of a BlockLayout box.
 *
 * Use-case: Separates the visual rendering logic (box-shadows, backgrounds,
 * gradients) from the spatial layout computation.
 */
class BlockPainter {
public:
  static void paint(const BlockLayout &layout,
                    std::vector<std::unique_ptr<DrawCommand>> &display_list);
};

} // namespace layout

#include "layout/LineLayout.h"

#include <algorithm>

LineLayout::LineLayout(const Lexeme *node, LayoutObject *parent,
                       LayoutObject *previous)
    : node_(node), parent_(parent), previous_(previous) {}

void LineLayout::layout() {
  if (!parent_)
    return;

  width = parent_->width;
  x = parent_->x;

  if (previous_) {
    y = previous_->y + previous_->height;
  } else {
    y = parent_->y;
  }

  for (auto &child : children_) {
    child->layout();
  }

  if (children_.empty()) {
    // If a line is empty (like an isolated <br>), give it a default height
    // so it actually produces vertical spacing. 16px font * 1.25 line-height =
    // 20
    height = 20;
    return;
  }

  int max_ascent = 0;

  for (const auto &child : children_) {
    // Only TextLayouts sit inside LineLayout.
    // They don't expose ascent/descent directly since we don't store
    // FontMetrics on them. For simplicity, we just take the max height
    // as our line height, but properly we'd need the TTF_Font metrics.
    // Here we approximate based on child heights since child layout measured
    // it. To match BlockLayout's 1.25 multiplier:

    // As a simplification for now, since TextLayout height is just the bounding
    // box from TTF_SizeUTF8 (which includes ascent and descent), we'll do
    // simple stacking.
    max_ascent = std::max(max_ascent, child->height);
  }

  for (auto &child : children_) {
    // Top-align text within the line for now, to keep it simple without full
    // TTF metrics
    child->y = y;
  }

  height = static_cast<int>(max_ascent * 1.25);
}

void LineLayout::paint(std::vector<std::unique_ptr<DrawCommand>> &out) const {
  (void)out;
  // LineLayout itself is invisible.
  // Painting is entirely delegated to TextLayout children via paint_tree()
}

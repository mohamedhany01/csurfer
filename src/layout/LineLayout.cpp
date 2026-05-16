#include "layout/LineLayout.h"
#include "config/Config.h"

#include <algorithm>

LineLayout::LineLayout(const Lexeme *dom_node, LayoutObject *parent_layout,
                       LayoutObject *previous_sibling)
    : node_(dom_node), parent_(parent_layout), previous_(previous_sibling) {}

void LineLayout::layout() {
  if (!parent_)
    return;

  utils::Rect new_bounds;
  new_bounds.width = parent_->bounds().width;
  new_bounds.origin.x = parent_->bounds().origin.x;

  if (previous_) {
    new_bounds.origin.y =
        previous_->bounds().origin.y + previous_->bounds().height;
  } else {
    new_bounds.origin.y = parent_->bounds().origin.y;
  }
  set_bounds(new_bounds);

  for (auto &child : children_) {
    child->layout();
  }

  if (children_.empty()) {
    // Story: Empty lines (e.g., isolated <br>) need a default height to
    // produce vertical spacing.
    new_bounds = bounds();
    new_bounds.height = config::DEFAULT_LINE_HEIGHT;
    set_bounds(new_bounds);
    return;
  }

  int max_child_height = 0;
  for (const auto &child : children_) {
    max_child_height = std::max(max_child_height, child->bounds().height);
  }

  for (auto &child : children_) {
    // Story: Top-align all children within the line.
    utils::Rect child_bounds = child->bounds();
    child_bounds.origin.y = bounds().origin.y;
    child->set_bounds(child_bounds);
  }

  new_bounds = bounds();
  new_bounds.height =
      static_cast<int>(max_child_height * config::LINE_HEIGHT_MULTIPLIER);
  set_bounds(new_bounds);
}

void LineLayout::paint(
    std::vector<std::unique_ptr<DrawCommand>> &display_list) const {
  (void)display_list;
  // Story: LineLayout is a logical container and has no visual representation.
}

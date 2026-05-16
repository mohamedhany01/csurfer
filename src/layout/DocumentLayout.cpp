#include "layout/DocumentLayout.h"
#include "config/Config.h"

DocumentLayout::DocumentLayout(const Element *node,
                               gfx::FontManager &font_manager,
                               int viewport_width)
    : node_(node), font_manager_(font_manager),
      viewport_width_(viewport_width) {}

void DocumentLayout::layout() {
  children_.clear();

  // The document has padding on all sides so text does not touch window edges.
  width = viewport_width_ - 2 * config::HORIZONTAL_STEP;
  x = config::HORIZONTAL_STEP;
  y = config::VERTICAL_STEP;

  if (!node_) {
    height = 0;
    return;
  }

  auto child =
      std::make_unique<BlockLayout>(node_, this, nullptr, font_manager_);
  children_.push_back(std::move(child));

  children_.front()->layout();
  height = children_.front()->height;
}

void DocumentLayout::paint(
    std::vector<std::unique_ptr<DrawCommand>> &out) const {
  (void)out;
}

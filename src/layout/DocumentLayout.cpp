#include "layout/DocumentLayout.h"
#include "layout/LayoutConstants.h"

DocumentLayout::DocumentLayout(const Element *node, FontMetrics metrics,
                               int viewport_width)
    : node_(node), metrics_(metrics), viewport_width_(viewport_width) {}

void DocumentLayout::layout() {
  children_.clear();

  // The document has padding on all sides so text does not touch window edges.
  width = viewport_width_ - 2 * HSTEP;
  x = HSTEP;
  y = VSTEP;

  if (!node_) {
    height = 0;
    return;
  }

  auto child = std::make_unique<BlockLayout>(node_, this, nullptr, metrics_);
  children_.push_back(std::move(child));

  children_.front()->layout();
  height = children_.front()->height;
}

void DocumentLayout::paint(
    std::vector<std::unique_ptr<DrawCommand>> &out) const {
  (void)out;
}

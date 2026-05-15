#include "layout/DocumentLayout.h"
#include "config/Config.h"

DocumentLayout::DocumentLayout(const Element *root_element,
                               gfx::FontManager &font_manager,
                               int viewport_width)
    : root_element_(root_element), font_manager_(font_manager),
      viewport_width_(viewport_width) {}

void DocumentLayout::layout() {
  children_.clear();

  utils::Rect new_bounds;
  // Story: The document has padding on all sides so content does not touch
  // edges.
  new_bounds.width = viewport_width_ - 2 * config::H_STEP;
  new_bounds.origin.x = config::H_STEP;
  new_bounds.origin.y = config::V_STEP;
  set_bounds(new_bounds);

  if (!root_element_) {
    new_bounds = bounds();
    new_bounds.height = 0;
    set_bounds(new_bounds);
    return;
  }

  auto body_layout = std::make_unique<BlockLayout>(root_element_, this, nullptr,
                                                   font_manager_);
  add_child(std::move(body_layout));

  children_.front()->layout();

  new_bounds = bounds();
  new_bounds.height = children_.front()->bounds().height;
  set_bounds(new_bounds);
}

void DocumentLayout::paint(
    std::vector<std::unique_ptr<DrawCommand>> &display_list) const {
  (void)display_list;
}

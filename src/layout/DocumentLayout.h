#pragma once
#include "layout/BlockLayout.h"
#include "dom/Element.h"

/**
 * Story: The root of the layout tree, representing the entire document view.
 *
 * Use-case: It initializes the layout process by setting the initial
 * viewport dimensions and triggering the recursive layout of the <body>.
 */
class DocumentLayout final : public LayoutObject {
public:
  DocumentLayout(const Element *root_element, gfx::FontManager &font_manager,
                 int viewport_width);

  void layout() override;
  void
  paint(std::vector<std::unique_ptr<DrawCommand>> &display_list) const override;

private:
  const Element *root_element_;
  gfx::FontManager &font_manager_;
  int viewport_width_;
};

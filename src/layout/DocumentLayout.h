#pragma once
#include "gfx/Font.h"
#include "layout/BlockLayout.h"
#include "layout/LayoutObject.h"
#include "lexer/Element.h"

// Root of the layout tree for a document.
//
// This node represents the full page. It creates a single BlockLayout child
// for the HTML root element and gives it a padded content box.
//
// Example:
//   DocumentLayout doc(root, font_manager, WIDTH);
//   doc.layout();
//   std::vector<std::unique_ptr<DrawCommand>> display_list;
//   paint_tree(doc, display_list);
class DocumentLayout final : public LayoutObject {
public:
  // Create a document layout for the given HTML root and viewport width.
  // The viewport width is the total window width; padding is applied inside.
  DocumentLayout(const Element *node, gfx::FontManager &font_manager,
                 int viewport_width);

  // Compute x, y, width and height for this document and its child.
  void layout() override;

  // Document itself does not draw anything; children handle painting.
  void paint(std::vector<std::unique_ptr<DrawCommand>> &out) const override;

private:
  const Element *node_;
  gfx::FontManager &font_manager_;
  int viewport_width_;
};

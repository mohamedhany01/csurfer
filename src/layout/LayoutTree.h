#pragma once

#include "layout/LayoutObject.h"
#include <memory>
#include <vector>

// Walk the layout tree and collect all DrawCommand objects.
//
// Example:
//   DocumentLayout doc(root, metrics, WIDTH);
//   doc.layout();
//   std::vector<std::unique_ptr<DrawCommand>> display_list;
//   paint_tree(doc, display_list);
inline void
paint_tree(const LayoutObject &layout_object,
           std::vector<std::unique_ptr<DrawCommand>> &display_list) {
  layout_object.paint(display_list);
  for (const auto &child : layout_object.children_) {
    paint_tree(*child, display_list);
  }
}

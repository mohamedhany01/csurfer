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

// Flatten the layout tree into a list for hit-testing.
// Example: auto all = tree_to_list(*document_);
inline std::vector<const LayoutObject*>
tree_to_list(const LayoutObject& root) {
  std::vector<const LayoutObject*> list;
  std::vector<const LayoutObject*> stack = {&root};
  while (!stack.empty()) {
    const LayoutObject* node = stack.back();
    stack.pop_back();
    list.push_back(node);
    
    // push children in reverse so they are popped in order
    for (auto it = node->children_.rbegin(); it != node->children_.rend(); ++it) {
      stack.push_back(it->get());
    }
  }
  return list;
}

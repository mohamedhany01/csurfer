#pragma once

#include "layout/LayoutObject.h"
#include "lexer/Element.h"
#include "lexer/Lexeme.h"
#include "lexer/Text.h"
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
  float opacity = layout_object.get_opacity();
  if (opacity < 1.0f) {
    display_list.push_back(std::make_unique<DrawSaveLayer>(opacity));
  }

  layout_object.paint(display_list);
  for (const auto &child : layout_object.children_) {
    paint_tree(*child, display_list);
  }

  if (opacity < 1.0f) {
    display_list.push_back(std::make_unique<DrawRestore>());
  }
}

// Flatten the layout tree into a list for hit-testing.
// Example: auto all = tree_to_list(*document_);
inline std::vector<const LayoutObject *>
tree_to_list(const LayoutObject &root) {
  std::vector<const LayoutObject *> list;
  std::vector<const LayoutObject *> stack = {&root};
  while (!stack.empty()) {
    const LayoutObject *node = stack.back();
    stack.pop_back();
    list.push_back(node);

    // push children in reverse so they are popped in order
    for (auto it = node->children_.rbegin(); it != node->children_.rend();
         ++it) {
      stack.push_back(it->get());
    }
  }
  return list;
}

#include <iostream>
inline void debug_print_layout_tree(const LayoutObject &node, int indent = 0) {
  std::cout << std::string(indent, ' ') << "Node: bounds (" << node.x << ","
            << node.y << "," << node.width << "," << node.height << ") ";
  if (const LayoutObject *cnode = &node) {
    if (const Lexeme *dom_node = cnode->node()) {
      if (dom_node->type() == LexemeType::Element) {
        std::cout << "(Element " << dom_node->element() << ") "; // Hack
      } else {
        std::cout << "(Text '" << dom_node->text() << "')";
      }
    }
  }
  std::cout << "\n";
  for (const auto &child : node.children_) {
    debug_print_layout_tree(*child, indent + 2);
  }
}

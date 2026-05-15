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
  std::string blend_mode = layout_object.get_blend_mode();
  bool is_clip = layout_object.is_overflow_clip();
  float border_radius = layout_object.get_border_radius();

  bool should_save = (opacity < 1.0f || !blend_mode.empty() || is_clip);

  if (should_save) {
    display_list.push_back(
        std::make_unique<DrawSaveLayer>(opacity, blend_mode));
  }

  layout_object.paint(display_list);
  for (const auto &child : layout_object.children_) {
    paint_tree(*child, display_list);
  }

  if (is_clip) {
    // The "destination-in" hack: we push a new layer with destination-in blend
    // mode. Then we draw a white rounded rectangle. The intersection of this
    // white rect with everything drawn inside the element will be preserved,
    // and everything outside the rounded rectangle will be erased (clipped).
    display_list.push_back(
        std::make_unique<DrawSaveLayer>(1.0f, "destination-in"));
    display_list.push_back(std::make_unique<DrawRoundedRect>(
        layout_object.bounds, border_radius, gfx::Color::White()));
    display_list.push_back(std::make_unique<DrawRestore>());
  }

  if (should_save) {
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
  std::cout << std::string(indent, ' ') << "Node: bounds ("
            << node.bounds.origin.x << "," << node.bounds.origin.y << ","
            << node.bounds.width << "," << node.bounds.height << ") ";
  if (const LayoutObject *cnode = &node) {
    if (const Lexeme *dom_node = cnode->node()) {
      if (dom_node->type() == LexemeType::Element) {
        std::cout << "(Element " << dom_node->tag() << ") "; // Hack
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

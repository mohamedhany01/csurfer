#pragma once

#include "dom/Element.h"
#include "dom/Lexeme.h"
#include "dom/Text.h"
#include "layout/LayoutObject.h"
#include <memory>
#include <vector>

/**
 * Story: Recursively paints the entire layout tree into a display list.
 *
 * Use-case: This function handles the visual stacking and effects (like
 * opacity, clipping, and blend modes) by wrapping node painting in
 * SaveLayer/Restore blocks.
 */
inline void
paint_tree(const LayoutObject &layout_object,
           std::vector<std::unique_ptr<DrawCommand>> &display_list) {
  float opacity = layout_object.get_opacity();
  std::string blend_mode = layout_object.get_blend_mode();
  bool is_clipping = layout_object.is_overflow_clip();
  float border_radius = layout_object.get_border_radius();

  bool needs_layer_save =
      (opacity < 1.0f || !blend_mode.empty() || is_clipping);

  if (needs_layer_save) {
    display_list.push_back(
        std::make_unique<DrawSaveLayer>(opacity, blend_mode));
  }

  layout_object.paint(display_list);
  for (const auto &child : layout_object.children()) {
    paint_tree(*child, display_list);
  }

  if (is_clipping) {
    // Story: The "destination-in" hack for rounded-rect clipping.
    display_list.push_back(
        std::make_unique<DrawSaveLayer>(1.0f, "destination-in"));
    display_list.push_back(std::make_unique<DrawRoundedRect>(
        layout_object.bounds(), border_radius, gfx::Color::White()));
    display_list.push_back(std::make_unique<DrawRestore>());
  }

  if (needs_layer_save) {
    display_list.push_back(std::make_unique<DrawRestore>());
  }
}

/**
 * Story: Flattens the tree into a list for fast iteration (e.g., hit-testing).
 */
inline std::vector<const LayoutObject *>
tree_to_list(const LayoutObject &root_node) {
  std::vector<const LayoutObject *> flattened_list;
  std::vector<const LayoutObject *> stack = {&root_node};
  while (!stack.empty()) {
    const LayoutObject *current_node = stack.back();
    stack.pop_back();
    flattened_list.push_back(current_node);

    // Push children in reverse so they are processed in order
    const auto &children = current_node->children();
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
      stack.push_back(it->get());
    }
  }
  return flattened_list;
}

#include "layout/BlockLayout.h"
#include "config/Config.h"
#include "css/CSSUtils.h"
#include "dom/Text.h"
#include "layout/BlockPainter.h"
#include "layout/InlineLayoutHelper.h"
#include "layout/InputLayout.h"
#include "layout/LineLayout.h"
#include "layout/TextLayout.h"
#include "utils/Logger.h"
#include "utils/Parser.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <unordered_set>

static const std::unordered_set<std::string> BLOCK_ELEMENTS = {
    "html",   "body",       "article", "section",  "nav",        "aside",
    "h1",     "h2",         "h3",      "h4",       "h5",         "h6",
    "hgroup", "header",     "footer",  "address",  "p",          "hr",
    "pre",    "blockquote", "ol",      "ul",       "menu",       "li",
    "dl",     "dt",         "dd",      "figure",   "figcaption", "main",
    "div",    "table",      "form",    "fieldset", "legend",     "details",
    "summary"};

BlockLayout::BlockLayout(const Lexeme *dom_node, LayoutObject *parent_layout,
                         BlockLayout *previous_sibling,
                         gfx::FontManager &font_manager)
    : node_(dom_node), parent_layout_(parent_layout),
      previous_sibling_(previous_sibling), font_manager_(font_manager),
      current_cursor_x_(0) {}

BlockLayout::BlockLayout(std::vector<const Lexeme *> anonymous_children,
                         LayoutObject *parent_layout,
                         BlockLayout *previous_sibling,
                         gfx::FontManager &font_manager)
    : node_(nullptr), parent_layout_(parent_layout),
      previous_sibling_(previous_sibling), font_manager_(font_manager),
      anonymous_children_(std::move(anonymous_children)), current_cursor_x_(0) {
}

float BlockLayout::get_opacity() const {
  if (const auto *element = dynamic_cast<const Element *>(node_)) {
    auto styles = element->style();
    if (styles.find("opacity") != styles.end()) {
      try {
        return std::stof(styles.at("opacity"));
      } catch (const std::exception &e) {
        CS_LOG_ERROR("Failed to parse opacity: {}", e.what());
        return 1.0f;
      }
    }
  }
  return 1.0f;
}

std::string BlockLayout::get_blend_mode() const {
  if (const auto *element = dynamic_cast<const Element *>(node_)) {
    auto styles = element->style();
    if (styles.find("mix-blend-mode") != styles.end()) {
      return styles.at("mix-blend-mode");
    }
  }
  return "";
}

bool BlockLayout::is_overflow_clip() const {
  if (const auto *element = dynamic_cast<const Element *>(node_)) {
    auto styles = element->style();
    if (styles.find("overflow") != styles.end() &&
        styles.at("overflow") == "clip") {
      return true;
    }
  }
  return false;
}

float BlockLayout::get_border_radius() const {
  if (const auto *element = dynamic_cast<const Element *>(node_)) {
    auto styles = element->style();
    if (styles.find("border-radius") != styles.end()) {
      std::string radius_string = styles.at("border-radius");
      if (radius_string.find("px") != std::string::npos) {
        radius_string = radius_string.substr(0, radius_string.find("px"));
      }
      try {
        return std::stof(radius_string);
      } catch (const std::exception &e) {
        CS_LOG_ERROR("Failed to parse border-radius: {}", e.what());
        return 0.0f;
      }
    }
  }
  return 0.0f;
}

static bool is_block_node(const Lexeme *node) {
  if (!node)
    return false;
  if (node->type() == LexemeType::Text)
    return false;
  if (const auto *element = dynamic_cast<const Element *>(node)) {
    return BLOCK_ELEMENTS.contains(element->tag());
  }
  return false;
}

BlockLayout::LayoutMode BlockLayout::determine_layout_mode() const {
  if (!node_) {
    if (!anonymous_children_.empty())
      return LayoutMode::Inline;
    return LayoutMode::Block;
  }

  if (node_->type() == LexemeType::Text) {
    return LayoutMode::Inline;
  }

  const auto *element = dynamic_cast<const Element *>(node_);
  if (!element)
    return LayoutMode::Block;

  bool has_block_child = false;
  for (const auto &child : element->children()) {
    if (child && child->type() == LexemeType::Element) {
      const auto *child_element = dynamic_cast<const Element *>(child.get());
      if (child_element && BLOCK_ELEMENTS.contains(child_element->tag())) {
        has_block_child = true;
        break;
      }
    }
  }

  if (has_block_child)
    return LayoutMode::Block;

  if (!element->children().empty() || element->tag() == "input" ||
      element->tag() == "button")
    return LayoutMode::Inline;

  return LayoutMode::Block;
}

/**
 * Story: The core layout algorithm for block-level elements.
 * This determines the bounds of the element and positions its children
 * based on whether they are block or inline.
 */
void BlockLayout::layout() {
  children_.clear();

  // Story: Initialize bounds relative to the parent or previous sibling.
  utils::Rect new_bounds;
  new_bounds.origin.x = parent_layout_ ? parent_layout_->bounds().origin.x : 0;
  new_bounds.width = parent_layout_ ? parent_layout_->bounds().width : 0;

  if (previous_sibling_) {
    new_bounds.origin.y = previous_sibling_->bounds().origin.y +
                          previous_sibling_->bounds().height;
  } else {
    new_bounds.origin.y =
        parent_layout_ ? parent_layout_->bounds().origin.y : 0;
  }
  set_bounds(new_bounds);

  const auto mode = determine_layout_mode();
  if (mode == LayoutMode::Block) {
    layout_block_children();
  } else {
    layout_inline_children();
  }

  // Story: Recursively layout all children.
  for (auto &child : children_) {
    child->layout();
  }

  // Story: Finalize own height based on the total height of children.
  int total_height = 0;
  for (const auto &child : children_) {
    total_height += child->bounds().height;
  }
  new_bounds = bounds();
  new_bounds.height = total_height;
  set_bounds(new_bounds);
}

/**
 * Story: Handles elements with 'Block' layout mode.
 * It groups consecutive inline elements into 'Anonymous Block' containers
 * to maintain the block formatting context.
 */
void BlockLayout::layout_block_children() {
  const auto *element = dynamic_cast<const Element *>(node_);
  if (!element)
    return;

  BlockLayout *previous_child = nullptr;
  std::vector<const Lexeme *> inline_run;

  auto flush_inline_run = [&]() {
    if (!inline_run.empty()) {
      auto anonymous_block = std::make_unique<BlockLayout>(
          inline_run, this, previous_child, font_manager_);
      previous_child = anonymous_block.get();
      add_child(std::move(anonymous_block));
      inline_run.clear();
    }
  };

  for (const auto &child : element->children()) {
    const Lexeme *child_node = child.get();
    if (child_node->type() == LexemeType::Element) {
      const auto *child_element = dynamic_cast<const Element *>(child_node);
      std::string tag = child_element->tag();
      if (tag == "head" || tag == "script" || tag == "style" || tag == "meta" ||
          tag == "link") {
        continue;
      }
    }

    if (is_block_node(child_node)) {
      flush_inline_run();
      auto block_child = std::make_unique<BlockLayout>(
          child_node, this, previous_child, font_manager_);
      previous_child = block_child.get();
      add_child(std::move(block_child));
    } else {
      inline_run.push_back(child_node);
    }
  }
  flush_inline_run();
}

/**
 * Story: Handles elements with 'Inline' layout mode.
 * It uses a cursor-based approach to flow inline elements (text, inputs)
 * into LineLayout containers, performing line-wrapping when necessary.
 */
void BlockLayout::layout_inline_children() {
  layout::InlineLayoutHelper::layout(*this);
}

void BlockLayout::paint(
    std::vector<std::unique_ptr<DrawCommand>> &display_list) const {
  layout::BlockPainter::paint(*this, display_list);
}

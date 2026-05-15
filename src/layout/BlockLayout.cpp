#include "layout/BlockLayout.h"

#include "layout/InputLayout.h"
#include "layout/LineLayout.h"
#include "layout/TextLayout.h"
#include "dom/Text.h"
#include "utils/Parser.h"
#include <algorithm>
#include <cmath>
#include <iostream>
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

/**
 * Story: A simple parser for CSS linear gradients.
 */
static bool parse_linear_gradient(const std::string &value,
                                  std::string &direction, gfx::Color &color1,
                                  gfx::Color &color2) {
  if (value.find("linear-gradient(") != 0)
    return false;

  size_t start_index = 16; // length of "linear-gradient("
  size_t end_index = value.find_last_of(')');
  if (end_index == std::string::npos || end_index <= start_index)
    return false;

  std::string content = value.substr(start_index, end_index - start_index);
  std::stringstream ss(content);
  std::string part;
  std::vector<std::string> parts;

  while (std::getline(ss, part, ',')) {
    // Trim whitespace
    size_t first = part.find_first_not_of(" \t");
    size_t last = part.find_last_not_of(" \t");
    if (first != std::string::npos) {
      parts.push_back(part.substr(first, last - first + 1));
    }
  }

  if (parts.size() < 2)
    return false;

  if (parts.size() == 2) {
    direction = "to bottom";
    color1 = gfx::Color::from_name(parts[0]);
    color2 = gfx::Color::from_name(parts[1]);
    return true;
  }
  if (parts.size() == 3) {
    direction = parts[0];
    color1 = gfx::Color::from_name(parts[1]);
    color2 = gfx::Color::from_name(parts[2]);
  }

  return true;
}

float BlockLayout::get_opacity() const {
  if (const auto *element = dynamic_cast<const Element *>(node_)) {
    auto styles = element->style();
    if (styles.find("opacity") != styles.end()) {
      try {
        return std::stof(styles.at("opacity"));
      } catch (...) {
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
      } catch (...) {
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

void BlockLayout::layout() {
  children_.clear();

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
    const auto *element = dynamic_cast<const Element *>(node_);
    if (element) {
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
          if (tag == "head" || tag == "script" || tag == "style" ||
              tag == "meta" || tag == "link") {
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
  } else {
    current_cursor_x_ = 0;
    start_new_line();
    if (!anonymous_children_.empty()) {
      for (const auto *node : anonymous_children_) {
        recurse_node(node);
      }
    } else {
      recurse_node(node_);
    }
  }

  for (auto &child : children_) {
    child->layout();
  }

  int total_height = 0;
  for (const auto &child : children_) {
    total_height += child->bounds().height;
  }
  new_bounds = bounds();
  new_bounds.height = total_height;
  set_bounds(new_bounds);
}

void BlockLayout::paint(
    std::vector<std::unique_ptr<DrawCommand>> &display_list) const {
  const auto *element = dynamic_cast<const Element *>(node_);
  if (element) {
    auto styles = element->style();

    float border_radius = 0.0f;
    if (styles.find("border-radius") != styles.end()) {
      std::string radius_string = styles.at("border-radius");
      if (radius_string.find("px") != std::string::npos) {
        radius_string = radius_string.substr(0, radius_string.find("px"));
      }
      try {
        border_radius = std::stof(radius_string);
      } catch (...) {
        border_radius = 0.0f;
      }
    }

    if (styles.find("box-shadow") != styles.end()) {
      std::string shadow_string = styles.at("box-shadow");
      std::stringstream ss(shadow_string);
      std::string dx_str, dy_str, blur_str, color_str;
      if (ss >> dx_str >> dy_str >> blur_str >> color_str) {
        try {
          int dx = std::stoi(dx_str);
          int dy = std::stoi(dy_str);
          int blur = std::stoi(blur_str);
          gfx::Color shadow_color = gfx::Color::from_name(color_str);
          display_list.push_back(std::make_unique<DrawBoxShadow>(
              utils::Rect{{bounds_.origin.x, bounds_.origin.y},
                          (int)bounds_.width,
                          (int)bounds_.height},
              (float)blur, dx, dy, shadow_color));
        } catch (...) {
        }
      }
    }

    if (styles.find("background-color") != styles.end()) {
      std::string background_color_name = styles.at("background-color");
      if (background_color_name != "transparent" &&
          !background_color_name.empty()) {
        gfx::Color color = gfx::Color::from_name(background_color_name);

        if (border_radius > 0.0f) {
          display_list.push_back(std::make_unique<DrawRoundedRect>(
              utils::Rect{{bounds_.origin.x, bounds_.origin.y},
                          (int)bounds_.width,
                          (int)bounds_.height},
              border_radius, color));
        } else {
          display_list.push_back(std::make_unique<DrawRect>(
              bounds_.origin.x, bounds_.origin.y,
              bounds_.origin.x + bounds_.width,
              bounds_.origin.y + bounds_.height, color));
        }
      }
    }

    if (styles.find("background") != styles.end()) {
      std::string background_value = styles.at("background");
      std::string gradient_direction;
      gfx::Color color_start, color_end;
      if (parse_linear_gradient(background_value, gradient_direction,
                                color_start, color_end)) {
        display_list.push_back(std::make_unique<DrawLinearGradient>(
            utils::Rect{{bounds_.origin.x, bounds_.origin.y},
                        (int)bounds_.width,
                        (int)bounds_.height},
            color_start, color_end, gradient_direction));
      }
    }
  }
}

void BlockLayout::recurse_node(const Lexeme *node) { layout_node(node); }

void BlockLayout::layout_node(const Lexeme *node) {
  if (!node)
    return;

  if (node->type() == LexemeType::Text) {
    const Element *parent_element = nullptr;
    if (const auto *text_node = dynamic_cast<const Text *>(node)) {
      parent_element = dynamic_cast<const Element *>(text_node->parent());
    }
    layout_text(node, node->text(), parent_element);
    return;
  }

  if (node->type() == LexemeType::Element) {
    const auto *element = dynamic_cast<const Element *>(node);
    if (element) {
      if (element->tag() == "br") {
        start_new_line();
      } else if (element->tag() == "input" || element->tag() == "button") {
        layout_input(element);
        return;
      }
    }
    layout_element(element);
  }
}

void BlockLayout::layout_element(const Element *element_node) {
  if (!element_node)
    return;

  for (const auto &child : element_node->children()) {
    layout_node(child.get());
  }
}

void BlockLayout::layout_text(const Lexeme *text_node,
                              const std::string &content,
                              const Element *parent_element) {
  auto words = utils::split_into_words(content);
  for (const auto &word_text : words) {
    if (word_text == "\n") {
      continue;
    }
    layout_word(text_node, word_text, parent_element);
  }
}

void BlockLayout::layout_word(const Lexeme *origin_node,
                              const std::string &word_text,
                              const Element *parent_element) {
  std::shared_ptr<gfx::Font> font = get_current_font(parent_element);
  if (!font)
    return;

  int word_width = 0;
  int word_height = 0;
  font->measure_text(word_text, word_width, word_height);

  if (current_cursor_x_ + word_width > bounds_.width) {
    start_new_line();
  }

  gfx::Color current_text_color = gfx::Color::Black();
  if (parent_element) {
    auto styles = parent_element->style();
    if (styles.find("color") != styles.end()) {
      current_text_color = gfx::Color::from_name(styles.at("color"));
    }
  }

  if (!children_.empty()) {
    LineLayout *current_line =
        dynamic_cast<LineLayout *>(children_.back().get());
    if (current_line) {
      auto text_layout = std::make_unique<TextLayout>(origin_node, word_text,
                                                      font, current_text_color);
      int space_width = 0;
      int space_height = 0;
      font->measure_text(" ", space_width, space_height);

      utils::Rect text_bounds = text_layout->bounds();
      text_bounds.origin.x = this->bounds_.origin.x + current_cursor_x_;
      text_layout->set_bounds(text_bounds);

      current_line->children().push_back(std::move(text_layout));
      current_cursor_x_ += word_width + space_width;
    }
  }
}

void BlockLayout::start_new_line() {
  current_cursor_x_ = 0;
  LayoutObject *previous_line = nullptr;
  if (!children_.empty()) {
    previous_line = children_.back().get();
  }

  add_child(std::make_unique<LineLayout>(node_, this, previous_line));
}

void BlockLayout::layout_input(const Lexeme *input_node) {
  const auto *element = dynamic_cast<const Element *>(input_node);
  if (!element)
    return;

  int input_width = 200; // Match DEFAULT_INPUT_WIDTH in InputLayout
  if (current_cursor_x_ + input_width > bounds_.width) {
    start_new_line();
  }

  if (!children_.empty()) {
    LineLayout *current_line =
        dynamic_cast<LineLayout *>(children_.back().get());
    if (current_line) {
      std::shared_ptr<gfx::Font> font = get_current_font(element);

      gfx::Color text_color = gfx::Color::Black();
      auto styles = element->style();
      if (styles.count("color")) {
        text_color = gfx::Color::from_name(styles.at("color"));
      }

      LayoutObject *previous_object = nullptr;
      if (!current_line->children().empty()) {
        previous_object = current_line->children().back().get();
      }

      auto input_layout = std::make_unique<InputLayout>(
          input_node, current_line, previous_object, font, text_color);
      current_line->children().push_back(std::move(input_layout));

      int space_width = 0;
      int space_height = 0;
      if (font) {
        font->measure_text(" ", space_width, space_height);
      }
      current_cursor_x_ += input_width + space_width;
    }
  }
}

#include "config/Config.h"

std::shared_ptr<gfx::Font>
BlockLayout::get_current_font(const Element *element_node) {
  int font_size = config::DEFAULT_FONT_SIZE;
  bool is_bold = false;
  bool is_italic = false;

  if (element_node) {
    auto styles = element_node->style();
    if (styles.find("font-size") != styles.end()) {
      std::string font_size_string = styles.at("font-size");
      if (font_size_string.length() > 2 &&
          font_size_string.substr(font_size_string.length() - 2) == "px") {
        try {
          font_size = std::stoi(
              font_size_string.substr(0, font_size_string.length() - 2));
        } catch (...) {
        }
      }
    }
    if (styles.find("font-weight") != styles.end() &&
        styles.at("font-weight") == "bold") {
      is_bold = true;
    }
    if (styles.find("font-style") != styles.end() &&
        styles.at("font-style") == "italic") {
      is_italic = true;
    }
  }

  FontKey font_key{font_size, is_bold, is_italic};
  if (font_cache_.contains(font_key))
    return font_cache_[font_key];

  auto font = font_manager_.get_font("Ubuntu", font_size, is_bold, is_italic);
  font_cache_[font_key] = font;
  return font;
}

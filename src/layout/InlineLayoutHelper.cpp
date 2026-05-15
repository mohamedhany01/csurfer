#include "layout/InlineLayoutHelper.h"
#include "config/Config.h"
#include "dom/Element.h"
#include "dom/Text.h"
#include "layout/BlockLayout.h"
#include "layout/InputLayout.h"
#include "layout/LineLayout.h"
#include "layout/TextLayout.h"
#include "utils/Logger.h"
#include "utils/Parser.h"
#include <algorithm>

namespace layout {

void InlineLayoutHelper::layout(BlockLayout &container) {
  container.current_cursor_x_ = 0;
  start_new_line(container);

  if (!container.anonymous_children_.empty()) {
    for (const auto *node : container.anonymous_children_) {
      recurse_node(container, node);
    }
  } else {
    recurse_node(container, container.node_);
  }
}

void InlineLayoutHelper::recurse_node(BlockLayout &container,
                                      const Lexeme *node) {
  if (!node)
    return;

  if (node->type() == LexemeType::Text) {
    const Element *parent_element = nullptr;
    if (const auto *text_node = dynamic_cast<const Text *>(node)) {
      parent_element = dynamic_cast<const Element *>(text_node->parent());
    }
    layout_text(container, node, node->text(), parent_element);
    return;
  }

  if (node->type() == LexemeType::Element) {
    const auto *element = dynamic_cast<const Element *>(node);
    if (element) {
      if (element->tag() == "br") {
        start_new_line(container);
      } else if (element->tag() == "input" || element->tag() == "button") {
        layout_input(container, element);
        return;
      }

      for (const auto &child : element->children()) {
        recurse_node(container, child.get());
      }
    }
  }
}

void InlineLayoutHelper::layout_text(BlockLayout &container,
                                     const Lexeme *text_node,
                                     const std::string &content,
                                     const Element *parent_element) {
  auto words = utils::split_into_words(content);
  for (const auto &word_text : words) {
    if (word_text == "\n") {
      continue;
    }
    layout_word(container, text_node, word_text, parent_element);
  }
}

void InlineLayoutHelper::layout_word(BlockLayout &container,
                                     const Lexeme *origin_node,
                                     const std::string &word_text,
                                     const Element *parent_element) {
  std::shared_ptr<gfx::Font> font = get_current_font(container, parent_element);
  if (!font)
    return;

  int word_width = 0;
  int word_height = 0;
  font->measure_text(word_text, word_width, word_height);

  if (container.current_cursor_x_ + word_width > container.bounds_.width) {
    start_new_line(container);
  }

  gfx::Color current_text_color = gfx::Color::Black();
  if (parent_element) {
    auto styles = parent_element->style();
    if (styles.find("color") != styles.end()) {
      current_text_color = gfx::Color::from_name(styles.at("color"));
    }
  }

  if (!container.children_.empty()) {
    LineLayout *current_line =
        dynamic_cast<LineLayout *>(container.children_.back().get());
    if (current_line) {
      auto text_layout = std::make_unique<TextLayout>(origin_node, word_text,
                                                      font, current_text_color);
      int space_width = 0;
      int space_height = 0;
      font->measure_text(" ", space_width, space_height);

      utils::Rect text_bounds = text_layout->bounds();
      text_bounds.origin.x =
          container.bounds_.origin.x + container.current_cursor_x_;
      text_layout->set_bounds(text_bounds);

      current_line->children().push_back(std::move(text_layout));
      container.current_cursor_x_ += word_width + space_width;
    }
  }
}

void InlineLayoutHelper::start_new_line(BlockLayout &container) {
  container.current_cursor_x_ = 0;
  LayoutObject *previous_line = nullptr;
  if (!container.children_.empty()) {
    previous_line = container.children_.back().get();
  }

  container.add_child(
      std::make_unique<LineLayout>(container.node_, &container, previous_line));
}

void InlineLayoutHelper::layout_input(BlockLayout &container,
                                      const Lexeme *input_node) {
  const auto *element = dynamic_cast<const Element *>(input_node);
  if (!element)
    return;

  int input_width = config::DEFAULT_INPUT_WIDTH;
  if (container.current_cursor_x_ + input_width > container.bounds_.width) {
    start_new_line(container);
  }

  if (!container.children_.empty()) {
    LineLayout *current_line =
        dynamic_cast<LineLayout *>(container.children_.back().get());
    if (current_line) {
      std::shared_ptr<gfx::Font> font = get_current_font(container, element);

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
      container.current_cursor_x_ += input_width + space_width;
    }
  }
}

std::shared_ptr<gfx::Font>
InlineLayoutHelper::get_current_font(BlockLayout &container,
                                     const Element *element_node) {
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
        } catch (const std::exception &e) {
          CS_LOG_ERROR("Failed to parse font-size: {}", e.what());
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
  if (container.font_cache_.contains(font_key))
    return container.font_cache_[font_key];

  auto font =
      container.font_manager_.get_font("Inter", font_size, is_bold, is_italic);
  container.font_cache_[font_key] = font;
  return font;
}

} // namespace layout

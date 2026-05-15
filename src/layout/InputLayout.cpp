#include "layout/InputLayout.h"
#include "dom/Element.h"
#include "layout/DisplayItem.h"

InputLayout::InputLayout(const Lexeme *dom_node, LayoutObject *parent_layout,
                         LayoutObject *previous_sibling,
                         std::shared_ptr<gfx::Font> font_handle,
                         gfx::Color text_color)
    : node_(dom_node), parent_(parent_layout), previous_(previous_sibling),
      font_(std::move(font_handle)), color_(text_color) {}

void InputLayout::layout() {
  utils::Rect new_bounds;
  new_bounds.width = DEFAULT_INPUT_WIDTH;

  // Position relative to previous element in the same line
  if (previous_) {
    int space_width = 0;
    int height = 0;
    if (font_) {
      font_->measure_text(" ", space_width, height);
    }
    new_bounds.origin.x =
        previous_->bounds().origin.x + previous_->bounds().width + space_width;
  } else {
    new_bounds.origin.x = parent_ ? parent_->bounds().origin.x : 0;
  }

  // Height is determined by the font line skip
  new_bounds.height = font_ ? font_->get_height() : config::DEFAULT_FONT_SIZE;
  set_bounds(new_bounds);
}

void InputLayout::paint(
    std::vector<std::unique_ptr<DrawCommand>> &display_list) const {
  const auto *element = dynamic_cast<const Element *>(node_);
  if (!element)
    return;

  // 1. Draw background/border of the input/button
  gfx::Color border_color = gfx::Color::from_rgb(160, 160, 160); // Light Gray
  gfx::Color background_color = gfx::Color::from_rgb(240, 240, 240); // Default

  if (element->tag() == "input") {
    background_color = gfx::Color::from_rgb(173, 216, 230); // Light Blue
  } else if (element->tag() == "button") {
    background_color = gfx::Color::from_rgb(255, 165, 0); // Orange
  }

  if (node_ && node_->is_focused()) {
    border_color = gfx::Color::from_rgb(0, 0, 255); // Blue when focused
  }

  // Draw background
  display_list.push_back(std::make_unique<DrawRect>(
      bounds_.origin.x, bounds_.origin.y, bounds_.origin.x + bounds_.width,
      bounds_.origin.y + bounds_.height, background_color));

  // Draw simple border (4 lines)
  display_list.push_back(std::make_unique<DrawLine>(
      bounds_.origin.x, bounds_.origin.y, bounds_.origin.x + bounds_.width,
      bounds_.origin.y, border_color, 1));
  display_list.push_back(std::make_unique<DrawLine>(
      bounds_.origin.x, bounds_.origin.y + bounds_.height,
      bounds_.origin.x + bounds_.width, bounds_.origin.y + bounds_.height,
      border_color, 1));
  display_list.push_back(std::make_unique<DrawLine>(
      bounds_.origin.x, bounds_.origin.y, bounds_.origin.x,
      bounds_.origin.y + bounds_.height, border_color, 1));
  display_list.push_back(std::make_unique<DrawLine>(
      bounds_.origin.x + bounds_.width, bounds_.origin.y,
      bounds_.origin.x + bounds_.width, bounds_.origin.y + bounds_.height,
      border_color, 1));

  // 2. Determine display text
  std::string display_text;
  if (element->tag() == "input") {
    auto attributes = element->attributes();
    if (attributes.count("value")) {
      display_text = attributes.at("value");
    }
  } else if (element->tag() == "button") {
    // For buttons, use the text content of the first child
    if (!element->children().empty()) {
      const auto &child = element->children().front();
      if (child->type() == LexemeType::Text) {
        display_text = child->text();
      }
    }
  }

  // 3. Draw the text
  if (!display_text.empty()) {
    display_list.push_back(std::make_unique<DrawText>(
        bounds_.origin.x + config::DEFAULT_INPUT_PADDING, bounds_.origin.y,
        display_text, font_, color_));
  }

  // 4. Draw Caret if focused
  if (node_ && node_->is_focused()) {
    int text_width = 0;
    int height = 0;
    if (font_) {
      font_->measure_text(display_text, text_width, height);
    }
    int caret_x = bounds_.origin.x + config::DEFAULT_INPUT_PADDING + text_width;
    display_list.push_back(std::make_unique<DrawLine>(
        caret_x, bounds_.origin.y + 2, caret_x,
        bounds_.origin.y + bounds_.height - 2, gfx::Color::Black(), 2));
  }
}

const Lexeme *InputLayout::node() const { return node_; }

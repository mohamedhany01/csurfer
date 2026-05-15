#include "layout/InputLayout.h"
#include "layout/DisplayItem.h"
#include "lexer/Element.h"

InputLayout::InputLayout(const Lexeme *node, LayoutObject *parent,
                         LayoutObject *previous,
                         std::shared_ptr<gfx::Font> font, gfx::Color color)
    : node_(node), parent_(parent), previous_(previous), font_(std::move(font)),
      color_(color) {}

void InputLayout::layout() {
  bounds.width = DEFAULT_INPUT_WIDTH;

  // Position relative to previous element in the same line
  if (previous_) {
    int space_w = 0;
    int h = 0;
    if (font_) {
      font_->measure_text(" ", space_w, h);
    }
    bounds.origin.x =
        previous_->bounds.origin.x + previous_->bounds.width + space_w;
  } else {
    bounds.origin.x = parent_ ? parent_->bounds.origin.x : 0;
  }

  // Height is determined by the font line skip
  bounds.height = font_ ? font_->get_height() : 16;
}

void InputLayout::paint(std::vector<std::unique_ptr<DrawCommand>> &out) const {
  const auto *el = dynamic_cast<const Element *>(node_);
  if (!el)
    return;

  // 1. Draw background/border of the input/button
  gfx::Color border_color = gfx::Color::from_rgb(160, 160, 160); // Light Gray
  gfx::Color bg_color = gfx::Color::from_rgb(240, 240, 240);     // Default

  if (el->tag() == "input") {
    bg_color = gfx::Color::from_rgb(173, 216, 230); // Light Blue
  } else if (el->tag() == "button") {
    bg_color = gfx::Color::from_rgb(255, 165, 0); // Orange
  }

  if (node_ && node_->is_focused()) {
    border_color = gfx::Color::from_rgb(0, 0, 255); // Blue when focused
  }

  // Draw background
  out.push_back(std::make_unique<DrawRect>(
      bounds.origin.x, bounds.origin.y, bounds.origin.x + bounds.width,
      bounds.origin.y + bounds.height, bg_color));

  // Draw simple border (4 lines)
  out.push_back(std::make_unique<DrawLine>(bounds.origin.x, bounds.origin.y,
                                           bounds.origin.x + bounds.width,
                                           bounds.origin.y, border_color, 1));
  out.push_back(std::make_unique<DrawLine>(
      bounds.origin.x, bounds.origin.y + bounds.height,
      bounds.origin.x + bounds.width, bounds.origin.y + bounds.height,
      border_color, 1));
  out.push_back(std::make_unique<DrawLine>(
      bounds.origin.x, bounds.origin.y, bounds.origin.x,
      bounds.origin.y + bounds.height, border_color, 1));
  out.push_back(std::make_unique<DrawLine>(
      bounds.origin.x + bounds.width, bounds.origin.y,
      bounds.origin.x + bounds.width, bounds.origin.y + bounds.height,
      border_color, 1));

  // 2. Determine display text
  std::string text;
  if (el->tag() == "input") {
    auto attrs = el->attributes();
    if (attrs.count("value")) {
      text = attrs.at("value");
    }
  } else if (el->tag() == "button") {
    // For buttons, use the text content of the first child
    if (!el->children().empty()) {
      const auto &child = el->children().front();
      if (child->type() == LexemeType::Text) {
        text = child->text();
      }
    }
  }

  // 3. Draw the text
  if (!text.empty()) {
    out.push_back(std::make_unique<DrawText>(
        bounds.origin.x + 4, bounds.origin.y, text, font_, color_));
  }

  // 4. Draw Caret if focused
  if (node_ && node_->is_focused()) {
    int text_w = 0;
    int h = 0;
    if (font_) {
      font_->measure_text(text, text_w, h);
    }
    int caret_x = bounds.origin.x + 4 + text_w;
    out.push_back(std::make_unique<DrawLine>(
        caret_x, bounds.origin.y + 2, caret_x,
        bounds.origin.y + bounds.height - 2, gfx::Color::Black(), 2));
  }
}

const Lexeme *InputLayout::node() const { return node_; }

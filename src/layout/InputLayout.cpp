#include "layout/InputLayout.h"
#include "layout/DisplayItem.h"
#include "lexer/Element.h"

InputLayout::InputLayout(const Lexeme *node, LayoutObject *parent,
                         LayoutObject *previous,
                         std::shared_ptr<gfx::Font> font, gfx::Color color)
    : node_(node), parent_(parent), previous_(previous), font_(std::move(font)),
      color_(color) {}

void InputLayout::layout() {
  width = DEFAULT_INPUT_WIDTH;

  // Position relative to previous element in the same line
  if (previous_) {
    int space_w = 0;
    int h = 0;
    if (font_) {
      font_->measure_text(" ", space_w, h);
    }
    x = previous_->x + previous_->width + space_w;
  } else {
    x = parent_ ? parent_->x : 0;
  }

  // Height is determined by the font line skip
  height = font_ ? font_->get_height() : 16;
}

void InputLayout::paint(std::vector<std::unique_ptr<DrawCommand>> &out) const {
  const auto *el = dynamic_cast<const Element *>(node_);
  if (!el)
    return;

  // 1. Draw background/border of the input/button
  gfx::Color border_color = gfx::Color::FromRGB(160, 160, 160); // Light Gray
  gfx::Color bg_color = gfx::Color::FromRGB(240, 240, 240);     // Default

  if (el->tag() == "input") {
    bg_color = gfx::Color::FromRGB(173, 216, 230); // Light Blue
  } else if (el->tag() == "button") {
    bg_color = gfx::Color::FromRGB(255, 165, 0); // Orange
  }

  if (node_ && node_->is_focused()) {
    border_color = gfx::Color::FromRGB(0, 0, 255); // Blue when focused
  }

  // Draw background
  out.push_back(
      std::make_unique<DrawRect>(x, y, x + width, y + height, bg_color));

  // Draw simple border (4 lines)
  out.push_back(
      std::make_unique<DrawLine>(x, y, x + width, y, border_color, 1));
  out.push_back(std::make_unique<DrawLine>(x, y + height, x + width, y + height,
                                           border_color, 1));
  out.push_back(
      std::make_unique<DrawLine>(x, y, x, y + height, border_color, 1));
  out.push_back(std::make_unique<DrawLine>(x + width, y, x + width, y + height,
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
    out.push_back(std::make_unique<DrawText>(x + 4, y, text, font_, color_));
  }

  // 4. Draw Caret if focused
  if (node_ && node_->is_focused()) {
    int text_w = 0;
    int h = 0;
    if (font_) {
      font_->measure_text(text, text_w, h);
    }
    int caret_x = x + 4 + text_w;
    out.push_back(std::make_unique<DrawLine>(
        caret_x, y + 2, caret_x, y + height - 2, gfx::Color::Black(), 2));
  }
}

const Lexeme *InputLayout::node() const { return node_; }

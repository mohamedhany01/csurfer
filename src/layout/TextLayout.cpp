#include "layout/TextLayout.h"

TextLayout::TextLayout(const Lexeme *dom_node, std::string word_text,
                       std::shared_ptr<gfx::Font> font_handle,
                       gfx::Color text_color)
    : node_(dom_node), word_(std::move(word_text)),
      font_(std::move(font_handle)), color_(text_color) {}

void TextLayout::layout() {
  if (!font_ || word_.empty()) {
    set_bounds({{0, 0}, 0, 0});
    return;
  }

  int width = 0;
  int height = 0;
  font_->measure_text(word_, width, height);

  utils::Rect new_bounds = bounds();
  new_bounds.width = width;
  new_bounds.height = height;
  set_bounds(new_bounds);
}

void TextLayout::paint(
    std::vector<std::unique_ptr<DrawCommand>> &display_list) const {
  if (bounds_.width > 0 && bounds_.height > 0 && font_) {
    display_list.push_back(std::make_unique<DrawText>(
        bounds_.origin.x, bounds_.origin.y, word_, font_, color_));
  }
}

const Lexeme *TextLayout::node() const { return node_; }

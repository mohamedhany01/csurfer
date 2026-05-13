#include "layout/TextLayout.h"

TextLayout::TextLayout(const Lexeme *node, std::string word,
                       std::shared_ptr<gfx::Font> font, gfx::Color color)
    : node_(node), word_(std::move(word)), font_(std::move(font)),
      color_(color) {}

void TextLayout::layout() {
  if (!font_ || word_.empty()) {
    width = 0;
    height = 0;
    return;
  }

  int w = 0;
  int h = 0;
  font_->measure_text(word_, w, h);

  width = w;
  height = h;
}

void TextLayout::paint(std::vector<std::unique_ptr<DrawCommand>> &out) const {
  if (width > 0 && height > 0 && font_) {
    out.push_back(std::make_unique<DrawText>(x, y, word_, font_, color_));
  }
}

const Lexeme *TextLayout::node() const { return node_; }

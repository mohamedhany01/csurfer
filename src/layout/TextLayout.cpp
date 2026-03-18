#include "layout/TextLayout.h"

TextLayout::TextLayout(const Lexeme *node, std::string word, TTF_Font *font,
                       SDL_Color color)
    : node_(node), word_(std::move(word)), font_(font), color_(color) {}

void TextLayout::layout() {
  if (!font_ || word_.empty()) {
    width = 0;
    height = 0;
    return;
  }

  // Measure the width and height of this specific word based on its font.
  // Note: We do NOT set x and y here! The parent LineLayout is responsible
  // for positioning us along the line and aligning our baseline.
  int w = 0;
  int h = 0;
  TTF_SizeUTF8(font_, word_.c_str(), &w, &h);

  width = w;
  height = h;
}

void TextLayout::paint(std::vector<std::unique_ptr<DrawCommand>> &out) const {
  if (width > 0 && height > 0 && font_) {
    out.push_back(std::make_unique<DrawText>(x, y, word_, font_, color_));
  }
}

const Lexeme *TextLayout::node() const { return node_; }

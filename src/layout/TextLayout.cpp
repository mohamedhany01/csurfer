#include "layout/TextLayout.h"
#include <SDL2/SDL_ttf.h>

TextLayout::TextLayout(const Lexeme *node, std::string word, void *font_handle,
                       gfx::Color color)
    : node_(node), word_(std::move(word)), font_handle_(font_handle),
      color_(color) {}

/**
 * Stage 1.2: layout() still uses SDL_ttf for measurement.
 * Decoupling measurement will follow in a later stage.
 */
void TextLayout::layout() {
  if (!font_handle_ || word_.empty()) {
    width = 0;
    height = 0;
    return;
  }

  TTF_Font *font = static_cast<TTF_Font *>(font_handle_);
  int w = 0;
  int h = 0;
  TTF_SizeUTF8(font, word_.c_str(), &w, &h);

  width = w;
  height = h;
}

void TextLayout::paint(std::vector<std::unique_ptr<DrawCommand>> &out) const {
  if (width > 0 && height > 0 && font_handle_) {
    out.push_back(
        std::make_unique<DrawText>(x, y, word_, font_handle_, color_));
  }
}

const Lexeme *TextLayout::node() const { return node_; }

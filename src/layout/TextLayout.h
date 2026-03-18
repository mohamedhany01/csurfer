#pragma once

#include "layout/DisplayItem.h"
#include "layout/LayoutObject.h"
#include "lexer/Lexeme.h"

#include <SDL2/SDL_ttf.h>
#include <string>

// Represents a single word of text in the layout tree.
//
// A TextLayout is a leaf node in the layout tree. It knows exactly which
// DOM node it came from, allowing us to hit-test a click back to the DOM.
//
// Example:
//   auto text = std::make_unique<TextLayout>(node, "Click", font, color);
//   text->layout(); // measures width/height
//   text->x = 50; text->y = 100; // set by parent LineLayout
class TextLayout final : public LayoutObject {
public:
  TextLayout(const Lexeme *node, std::string word, TTF_Font *font,
             SDL_Color color);

  void layout() override;
  void paint(std::vector<std::unique_ptr<DrawCommand>> &out) const override;
  const Lexeme *node() const override;

private:
  const Lexeme *node_;
  std::string word_;
  TTF_Font *font_;
  SDL_Color color_;
};

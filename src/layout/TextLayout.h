#pragma once

#include "layout/DisplayItem.h"
#include "layout/LayoutObject.h"
#include "lexer/Lexeme.h"

#include <string>

/**
 * Represents a single word of text in the layout tree.
 *
 * A TextLayout is a leaf node in the layout tree. It knows exactly which
 * DOM node it came from, allowing us to hit-test a click back to the DOM.
 */
class TextLayout final : public LayoutObject {
public:
  TextLayout(const Lexeme *node, std::string word, void *font_handle,
             gfx::Color color);

  void layout() override;
  void paint(std::vector<std::unique_ptr<DrawCommand>> &out) const override;
  const Lexeme *node() const override;

private:
  const Lexeme *node_;
  std::string word_;
  void *font_handle_;
  gfx::Color color_;
};

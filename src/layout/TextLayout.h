#pragma once
#include "dom/Lexeme.h"
#include "gfx/Font.h"
#include "layout/DisplayItem.h"
#include "layout/LayoutObject.h"

#include <memory>
#include <string>

/**
 * Represents a single word of text in the layout tree.
 *
 * A TextLayout is a leaf node in the layout tree. It knows exactly which
 * DOM node it came from, allowing us to hit-test a click back to the DOM.
 */
class TextLayout final : public LayoutObject {
public:
  TextLayout(const Lexeme *node, std::string word,
             std::shared_ptr<gfx::Font> font, gfx::Color color);

  void layout() override;
  void paint(std::vector<std::unique_ptr<DrawCommand>> &out) const override;
  const Lexeme *node() const override;

private:
  const Lexeme *node_;
  std::string word_;
  std::shared_ptr<gfx::Font> font_;
  gfx::Color color_;
};

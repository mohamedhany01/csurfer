#pragma once

#include "layout/DisplayItem.h"
#include "layout/LayoutObject.h"
#include "lexer/Lexeme.h"
#include <string>

/**
 * Layout object for interactive elements like <input> and <button>.
 * It handles the visual state (borders, text, caret) and sizing.
 */
class InputLayout final : public LayoutObject {
public:
  InputLayout(const Lexeme *node, LayoutObject *parent, LayoutObject *previous,
              void *font_handle, gfx::Color color);

  // Compute the size and relative position of the input box
  void layout() override;

  // Render the input box, text content, and focus caret
  void paint(std::vector<std::unique_ptr<DrawCommand>> &out) const override;

  // Return the underlying DOM node for hit-testing
  const Lexeme *node() const override;

private:
  const Lexeme *node_;
  LayoutObject *parent_;
  LayoutObject *previous_;
  void *font_handle_;
  gfx::Color color_;

  static constexpr int DEFAULT_INPUT_WIDTH = 200;
};

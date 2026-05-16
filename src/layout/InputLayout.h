#pragma once
#include "config/Config.h"
#include "gfx/Font.h"
#include "layout/DisplayItem.h"
#include "layout/LayoutObject.h"
#include "lexer/Lexeme.h"
#include <memory>
#include <string>

/**
 * Layout object for interactive elements like <input> and <button>.
 * It handles the visual state (borders, text, caret) and sizing.
 */
class InputLayout final : public LayoutObject {
public:
  InputLayout(const Lexeme *node, LayoutObject *parent, LayoutObject *previous,
              std::shared_ptr<gfx::Font> font, gfx::Color color);

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
  std::shared_ptr<gfx::Font> font_;
  gfx::Color color_;

  static constexpr int DEFAULT_INPUT_WIDTH = config::DEFAULT_INPUT_WIDTH;
};

#pragma once

#include "layout/LayoutObject.h"
#include "layout/TextLayout.h"

// Represents a single line of text within a block.
//
// A LineLayout manages a list of TextLayout children. It computes the
// baseline for the line based on the maximum ascent/descent of its children,
// and sets their absolute Y coordinates.
class LineLayout final : public LayoutObject {
public:
  LineLayout(const class Lexeme *node, LayoutObject *parent,
             LayoutObject *previous);

  void layout() override;
  void paint(std::vector<std::unique_ptr<DrawCommand>> &out) const override;

private:
  friend class BlockLayout;
  
  const Lexeme *node_;
  LayoutObject *parent_;
  LayoutObject *previous_;
};

#pragma once

#include <memory>
#include <vector>

class DrawCommand;

// Base class for all nodes in the layout tree.
//
// Each node has a rectangle (x, y, width, height) in page coordinates.
// Layout runs in two phases:
//   1. layout() computes sizes and positions.
//   2. paint() writes DrawCommand objects into the display list.
class LayoutObject {
public:
  virtual ~LayoutObject() = default;

  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;

  // Compute this node's box and recursively lay out children.
  virtual void layout() = 0;

  // Add this node's drawing commands to the output list.
  virtual void paint(std::vector<std::unique_ptr<DrawCommand>> &out) const = 0;

  // Return the DOM node this layout object was built from, or nullptr.
  // Overridden by TextLayout to enable hit-testing.
  virtual const class Lexeme* node() const { return nullptr; }

  const std::vector<std::unique_ptr<LayoutObject>> &children() const {
    return children_;
  }

protected:
  std::vector<std::unique_ptr<LayoutObject>> children_;
};

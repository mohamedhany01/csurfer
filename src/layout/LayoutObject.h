#pragma once
#include "utils/Geometry.h"
#include <memory>
#include <vector>

class DrawCommand;

// Base class for all nodes in the layout tree.
//
// Each node has a rectangle (bounds) in page coordinates.
// Layout runs in two phases:
//   1. layout() computes sizes and positions.
//   2. paint() writes DrawCommand objects into the display list.
class LayoutObject {
public:
  virtual ~LayoutObject() = default;

  utils::Rect bounds = {{0, 0}, 0, 0};

  // Compute this node's box and recursively lay out children.
  virtual void layout() = 0;

  // Add this node's drawing commands to the output list.
  virtual void paint(std::vector<std::unique_ptr<DrawCommand>> &out) const = 0;

  // Return the DOM node this layout object was built from, or nullptr.
  // Overridden by TextLayout to enable hit-testing.
  virtual const class Lexeme *node() const { return nullptr; }

  // Return the opacity of this layout object, typically parsed from CSS.
  // Returns 1.0f by default (fully opaque).
  virtual float get_opacity() const { return 1.0f; }
  virtual std::string get_blend_mode() const { return ""; }
  virtual bool is_overflow_clip() const { return false; }
  virtual float get_border_radius() const { return 0.0f; }

  std::vector<std::unique_ptr<LayoutObject>> children_;
};

#pragma once
#include "utils/Geometry.h"
#include <memory>
#include <vector>

class DrawCommand;

/**
 * Story: The base class for all nodes in the visual "Layout Tree".
 *
 * Use-case: While the DOM Tree represents the structure of the document,
 * the Layout Tree represents its visual geometry. Each node here
 * corresponds to a "box" on the screen.
 */
class LayoutObject {
public:
  virtual ~LayoutObject() = default;

  /**
   * Story: Computes the size and position of this node and its children.
   */
  virtual void layout() = 0;

  /**
   * Story: Records drawing commands into a display list.
   * This is separated from layout() to allow for efficient re-paints.
   */
  virtual void
  paint(std::vector<std::unique_ptr<DrawCommand>> &display_list) const = 0;

  /**
   * Story: Returns the DOM node that generated this layout object.
   */
  virtual const class Lexeme *node() const { return nullptr; }

  // Visual property accessors
  virtual float get_opacity() const { return 1.0f; }
  virtual std::string get_blend_mode() const { return ""; }
  virtual bool is_overflow_clip() const { return false; }
  virtual float get_border_radius() const { return 0.0f; }

  // Geometry accessors
  const utils::Rect &bounds() const { return bounds_; }
  void set_bounds(const utils::Rect &new_bounds) { bounds_ = new_bounds; }

  // Tree management
  const std::vector<std::unique_ptr<LayoutObject>> &children() const {
    return children_;
  }
  std::vector<std::unique_ptr<LayoutObject>> &children() { return children_; }

  void add_child(std::unique_ptr<LayoutObject> child) {
    children_.push_back(std::move(child));
  }

protected:
  utils::Rect bounds_ = {{0, 0}, 0, 0};
  std::vector<std::unique_ptr<LayoutObject>> children_;
};

#pragma once

namespace utils {

/**
 * Story: Represents a single pixel location on the screen.
 * Using a struct instead of two separate integers ensures that x and y
 * are always moved together as a single unit of data.
 *
 * Use-case: Used to represent mouse click locations and the top-left
 * corners of layout boxes.
 */
struct Point {
  int x;
  int y;

  bool operator==(const Point &other) const {
    return x == other.x && y == other.y;
  }
  bool operator!=(const Point &other) const { return !(*this == other); }
};

/**
 * Story: A rectangle defined by its top-left origin point and its dimensions.
 * In a web browser, every element is a box. This is our fundamental tool
 * for measuring and positioning everything on the screen.
 *
 * Use-case: Every HTML element (div, p, img) has a Rect that defines
 * its physical presence on the page.
 */
struct Rect {
  Point origin;
  int width;
  int height;

  /**
   * Story: Checks if a specific Point is within the bounds of this Rectangle.
   *
   * Use-case: Vital for interaction. When the user clicks the mouse, we use
   * this to determine which button or link was actually hit.
   */
  bool contains(Point point) const {
    return point.x >= origin.x && point.x <= origin.x + width &&
           point.y >= origin.y && point.y <= origin.y + height;
  }

  bool operator==(const Rect &other) const {
    return origin == other.origin && width == other.width &&
           height == other.height;
  }
  bool operator!=(const Rect &other) const { return !(*this == other); }
};

} // namespace utils

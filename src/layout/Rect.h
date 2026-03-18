#pragma once

/**
 * A simple Rectangle utility for hit-testing and UI layout.
 */
struct Rect {
  int x, y, width, height;

  bool contains(int px, int py) const {
    return px >= x && px < x + width &&
           py >= y && py < y + height;
  }
};

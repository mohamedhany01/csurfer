#pragma once

#include "layout/DisplayItem.h"
#include "layout/Rect.h"
#include "url/Url.h"
#include <string>
#include <vector>

class Browser; // Forward declaration

/**
 * CSurferUI handles the browser's graphical interface (GUI).
 * It renders the address bar, back button, and tabs.
 *
 * SOLID: Single Responsibility - Handles the browser's graphical interface.
 */
class CSurferUI {
public:
  explicit CSurferUI(Browser *browser);
  ~CSurferUI() = default;

  // Interaction
  void click(int x, int y);
  void keypress(SDL_Keycode key, const std::string &text);
  void enter();

  // Rendering
  void render(SDL_Renderer *renderer) const;

  // Metrics
  int height() const { return BOTTOM; }

private:
  Browser *browser_;

  // UI State
  std::string address_bar_text_;
  bool address_bar_focused_ = false;

  // Bounding boxes for hit-testing
  Rect back_button_rect_;
  Rect address_bar_rect_;
  Rect new_tab_rect_;

  // Visual Constants
  const int PADDING = 10;
  const int TAB_HEIGHT = 40;
  const int ADDR_HEIGHT = 40;
  const int BOTTOM = TAB_HEIGHT + ADDR_HEIGHT + 10; // Total UI height
};

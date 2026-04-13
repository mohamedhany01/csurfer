#pragma once

#include "js/JSContext.h"
#include "layout/DisplayItem.h"
#include "layout/DocumentLayout.h"
#include "lexer/Element.h"
#include "request/IRequest.h"
#include "url/Url.h"

#include <memory>
#include <string>
#include <vector>

/**
 * Represents a single browser tab containing its own URL, history,
 * DOM tree, and layout tree.
 *
 * SOLID: Single Responsibility - Handles the web page lifecycle.
 */
class Tab {
public:
  explicit Tab(std::shared_ptr<IRequest> http, int window_width,
               const FontMetrics &metrics);
  ~Tab();

  // Lifecycle
  void load(const Url &url, const std::string &payload = "");

  // Interaction
  void click(int x, int y);
  void handle_keypress(SDL_Keycode key, const std::string &text);
  void submit_form(const Element *form);
  void scrolldown();
  void scrollup();
  void go_back();
  void rebuild_layout();

  // Rendering
  // Paints the tab's display list into the output renderer, offset by y_offset
  void render(SDL_Renderer *renderer, int y_offset) const;

  // Getters
  const Url &url() const { return url_; }
  const std::string title() const; // TODO: extract from <title> tag
  Element *root() const { return root_.get(); }
  std::shared_ptr<IRequest> http() const { return http_; }

private:
  std::shared_ptr<IRequest> http_;
  int window_width_;
  FontMetrics metrics_;

  Url url_;
  std::vector<Url> history_;
  int scroll_ = 0;

  std::unique_ptr<Element> root_;
  std::unique_ptr<DocumentLayout> document_;
  std::unique_ptr<JSContext> js_;
  std::vector<std::unique_ptr<DrawCommand>> display_list_;

  Element *focus_ = nullptr;

  // Constants
  const int SCROLL_STEP = 100;
};

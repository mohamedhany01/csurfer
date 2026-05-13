#pragma once

#include "js/JSContext.h"
#include "layout/DisplayItem.h"
#include "layout/DocumentLayout.h"
#include "lexer/Element.h"
#include "request/IRequest.h"
#include "url/Url.h"
#include <SDL2/SDL.h>

#include <map>
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
               gfx::FontManager &font_manager);
  ~Tab();

  // Lifecycle
  void load(const Url &url, const std::string &payload = "");

  // Interaction
  void click(int x, int y);
  void handle_mousedown(int x, int y);
  void handle_mousemove(int x, int y);
  void handle_mouseup(int x, int y);
  void handle_keypress(SDL_Keycode key, const std::string &text);
  void submit_form(const Element *form);
  void scrolldown();
  void scrollup();
  void go_back();
  void rebuild_layout();

  // CSP
  void parse_csp(const std::string &header_value);
  bool is_allowed(const Url &url, const std::string &directive) const;

  // Rendering
  // Paints the tab's display list into the output renderer, offset by y_offset
  void render(gfx::GraphicsContext &ctx, int y_offset) const;

  // Getters
  const Url &url() const { return url_; }
  const std::string title() const; // TODO: extract from <title> tag
  Element *root() const { return root_.get(); }
  std::shared_ptr<IRequest> http() const { return http_; }

private:
  std::shared_ptr<IRequest> http_;
  int window_width_;
  gfx::FontManager &font_manager_;

  Url url_;
  std::vector<Url> history_;
  int scroll_ = 0;
  bool is_dragging_scrollbar_ = false;

  std::unique_ptr<Element> root_;
  std::unique_ptr<DocumentLayout> document_;
  std::unique_ptr<JSContext> js_;
  std::vector<std::unique_ptr<DrawCommand>> display_list_;

  Element *focus_ = nullptr;

  std::map<std::string, std::vector<std::string>> csp_directives_;

  void render_scrollbar(gfx::GraphicsContext &ctx, int y_offset) const;

  // Constants
  const int SCROLL_STEP = 100;
};

#pragma once

#include "dom/Element.h"
#include "js/IJSHost.h"
#include "js/JSContext.h"
#include "layout/DisplayItem.h"
#include "layout/DocumentLayout.h"
#include "request/IRequest.h"
#include "url/Url.h"
#include <SDL2/SDL.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

/**
 * Story: A single browser tab representing a web page.
 *
 * Use-case: Each tab maintains its own navigation history, DOM tree,
 * CSS styles, and JavaScript context. It handles the complete pipeline
 * from fetching a URL to rendering pixels on a display list.
 */
class Tab : public IJSHost {
public:
  explicit Tab(std::shared_ptr<IRequest> network_engine, int window_width,
               gfx::FontManager &font_manager);
  ~Tab();

  // Web Page Lifecycle
  void load(const Url &url, const std::string &payload = "");
  void load_error_page(const std::string &error_message);

  // Interaction delegation
  void click(int x, int y);
  void handle_mousedown(int x, int y);
  void handle_mousemove(int x, int y);
  void handle_mouseup(int x, int y);
  void handle_keypress(SDL_Keycode key, const std::string &text);

  void submit_form(const Element *form_element);
  void scroll_down();
  void scroll_up();
  void go_back();
  void rebuild_layout();

  // Security (CSP)
  void parse_csp(const std::string &header_value);
  bool is_allowed(const Url &target_url, const std::string &directive) const;

  /**
   * Story: Paints the tab's content into the graphics context.
   */
  void render(gfx::GraphicsContext &ctx, int y_screen_offset) const;

  // Accessors (IJSHost implementation)
  const Url &url() const override { return url_; }
  Element *root() const override { return root_.get(); }
  std::shared_ptr<IRequest> network_engine() const override {
    return network_engine_;
  }

  const std::string title() const;

private:
  std::shared_ptr<IRequest> network_engine_;
  int window_width_;
  gfx::FontManager &font_manager_;

  Url url_;
  std::vector<Url> history_;
  int current_scroll_ = 0;
  bool is_dragging_scrollbar_ = false;

  std::unique_ptr<Element> root_;
  std::unique_ptr<DocumentLayout> document_layout_;
  std::unique_ptr<JSContext> javascript_context_;
  std::vector<std::unique_ptr<DrawCommand>> display_list_;

  Element *focused_element_ = nullptr;

  std::map<std::string, std::vector<std::string>> csp_directives_;

  void render_scrollbar(gfx::GraphicsContext &ctx, int y_screen_offset) const;
};

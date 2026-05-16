#pragma once

#include "browser/TabInputHandler.h"
#include "browser/TabNavigator.h"
#include "browser/TabRenderer.h"
#include "browser/TabSecurityPolicy.h"
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
  void process_document(const std::string &content_or_message);

  // Interaction delegation
  void click(int x, int y);
  void handle_mousedown(int x, int y);
  void handle_mousemove(int x, int y);
  void handle_mouseup(int x, int y);
  void handle_keypress(SDL_Keycode key, const std::string &text);
  void scroll_down();
  void scroll_up();
  void go_back();
  void rebuild_layout();

  /**
   * Story: Paints the tab's content into the graphics context.
   */
  void render(gfx::GraphicsContext &ctx, int y_screen_offset) const;

  // Accessors (IJSHost implementation)
  const Url &url() const override { return navigator_.url(); }
  Element *root() const override { return root_.get(); }
  std::shared_ptr<IRequest> network_engine() const override {
    return navigator_.network_engine();
  }

  const std::string title() const;

private:
  int window_width_;
  gfx::FontManager &font_manager_;

  TabNavigator navigator_;
  TabRenderer renderer_;
  TabInputHandler input_handler_;

  std::unique_ptr<Element> root_;
  std::unique_ptr<DocumentLayout> document_layout_;
  std::unique_ptr<JSContext> javascript_context_;

  TabSecurityPolicy security_policy_;
  void process_scripts();
};

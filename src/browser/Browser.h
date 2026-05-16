#pragma once

#include "CSurferUI.h"
#include "IBrowserCommands.h"
#include "Tab.h"
#include "browser/SDLInitializer.h"
#include "config/Config.h"
#include "request/CookieJar.h"
#include "utils/Geometry.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <vector>

namespace gfx {
class SkiaContext;
class FontManager;
} // namespace gfx

/**
 * Story: The top-level orchestrator of the C Surfer browser.
 *
 * Use-case: This class manages the SDL2 window, the tab collection,
 * and the main execution loop. It acts as the "host" for both the
 * browser shell (UI) and the web content (Tabs).
 */
class Browser : public IBrowserCommands {
public:
  Browser();
  explicit Browser(std::shared_ptr<IRequest> network_engine);
  ~Browser();

  // Navigation Logic (IBrowserCommands implementation)
  void load(const std::string &raw_url) override;
  void load(const Url &url);

  // Interaction delegation
  void click(utils::Point point);
  void go_back() override;

  // Tab Management (IBrowserCommands implementation)
  void new_tab(const Url &url) override;
  void switch_to_tab(size_t index) override;
  void close_tab(size_t index) override;

  Tab *active_tab() const;
  Tab *get_tab(size_t index) const override { return tabs_[index].get(); }
  size_t tab_count() const override { return tabs_.size(); }
  size_t active_tab_index() const override { return active_tab_index_; }

  /**
   * Story: The heartbeat of the application.
   * Processes events and triggers re-renders until the window is closed.
   */
  void main_loop();

  TTF_Font *ui_font() const override { return ui_font_; }

private:
  SDLInitializer sdl_initializer_;
  std::shared_ptr<IRequest> network_engine_;

  // SDL2 Shell State
  const int width_ = config::WINDOW_WIDTH;
  const int height_ = config::WINDOW_HEIGHT;
  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;
  bool is_running_ = true;
  TTF_Font *ui_font_ = nullptr;

  // SDL2 Lifecycle helpers
  void load_ui_font();
  void create_window();
  void create_renderer();
  void shutdown();

  // Internal loop steps
  void handle_events();
  void draw();

  CSurferUI ui_;

  std::vector<std::unique_ptr<Tab>> tabs_;
  size_t active_tab_index_ = 0;

  // Graphics context for rendering page content using Skia/Freetype
  std::unique_ptr<gfx::SkiaContext> skia_ctx_;
  std::unique_ptr<gfx::FontManager> font_manager_;
  SDL_Texture *skia_texture_ = nullptr;

  CookieJar cookie_jar_;
};

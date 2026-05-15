#include "CSurferUI.h"
#include "Tab.h"
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

class Browser {
public:
  Browser();
  explicit Browser(std::shared_ptr<IRequest> http);
  ~Browser();

  // Load a URL, build the layout tree into the active tab.
  void load(const Url &url);

  // Interaction (delegated to tab)
  void click(utils::Point point);
  void go_back();

  // Tab Management
  void new_tab(const Url &url);
  void switch_to_tab(size_t index);
  void close_tab(size_t index);
  Tab *active_tab() const;
  Tab *get_tab(size_t index) const { return tabs_[index].get(); }
  size_t tab_count() const { return tabs_.size(); }
  size_t active_tab_index() const { return active_tab_index_; }

  // Finalize UI rendering and start the SDL event/render loop.
  void mainLoop();

  TTF_Font *get_font() const { return font; }

private:
  // Request
  std::shared_ptr<IRequest> http_;

  // SDL2 Shell
  const int WIDTH = config::WINDOW_WIDTH;
  const int HEIGHT = config::WINDOW_HEIGHT;
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  bool running = true;
  TTF_Font *font = nullptr;

  // SDL2 core
  void initSDL();
  void initTTF();
  void loadFont();
  void createWindow();
  void createRenderer();
  void shutdown();

  // SDL2 loop
  void handleEvents();
  void draw();

  // The Shell UI (CSurfer UI)
  CSurferUI ui_;

  // Tab Collection
  std::vector<std::unique_ptr<Tab>> tabs_;
  size_t active_tab_index_ = 0;

  // Graphics context for rendering page content using Skia
  std::unique_ptr<gfx::SkiaContext> skia_ctx_;
  std::unique_ptr<gfx::FontManager> font_manager_;
  SDL_Texture *skia_texture_ = nullptr;

  // Persistent State
  CookieJar cookie_jar_;
};

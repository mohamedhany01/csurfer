#include "Browser.h"
#include "gfx/SkiaContext.h"
#include "gfx/SkiaFont.h"
#include "request/HttpRequest.h"
#include <iostream>

Browser::Browser() : Browser(std::make_shared<HttpRequest>(&cookie_jar_)) {}

Browser::Browser(std::shared_ptr<IRequest> network_engine)
    : network_engine_(std::move(network_engine)), ui_(this) {
  init_sdl();
  init_ttf();
  load_ui_font();
  create_window();
  create_renderer();
  skia_ctx_ = std::make_unique<gfx::SkiaContext>(width_, height_);
  font_manager_ = std::make_unique<gfx::SkiaFontManager>();
}

Browser::~Browser() { shutdown(); }

void Browser::init_sdl() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    is_running_ = false;
  }
  SDL_StartTextInput(); // Story: Enable text input for the address bar and
                        // forms
}

void Browser::init_ttf() {
  if (TTF_Init() != 0) {
    std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
    is_running_ = false;
  }
}

void Browser::load_ui_font() {
  std::string font_path = std::string(ASSETS_DIR) + "/fonts/Ubuntu-Regular.ttf";
  ui_font_ = TTF_OpenFont(font_path.c_str(), 16);
  if (!ui_font_) {
    std::cerr << "Font error: " << TTF_GetError() << std::endl;
    is_running_ = false;
  }
}

void Browser::create_window() {
  window_ = SDL_CreateWindow("C Surfer \xf0\x9f\x8c\x8a",
                             SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                             width_, height_, SDL_WINDOW_SHOWN);
  if (!window_) {
    std::cerr << "Window error: " << SDL_GetError() << std::endl;
    is_running_ = false;
  }
}

void Browser::create_renderer() {
  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer_) {
    std::cerr << "Renderer error: " << SDL_GetError() << std::endl;
    is_running_ = false;
  }
}

void Browser::shutdown() {
  SDL_StopTextInput();
  if (skia_texture_)
    SDL_DestroyTexture(skia_texture_);
  if (renderer_)
    SDL_DestroyRenderer(renderer_);
  if (window_)
    SDL_DestroyWindow(window_);
  if (ui_font_)
    TTF_CloseFont(ui_font_);
  TTF_Quit();
  SDL_Quit();
}

void Browser::load(const std::string &raw_url) {
  if (!is_running_)
    return;

  try {
    load(Url(raw_url));
  } catch (const utils::UrlError &error) {
    std::cerr << "[Browser] URL Error: " << error.what() << std::endl;
    if (active_tab()) {
      active_tab()->load_error_page(error.what());
    }
  }
}

void Browser::load(const Url &url) {
  if (!is_running_)
    return;

  if (tabs_.empty()) {
    new_tab(url);
  } else {
    active_tab()->load(url);
  }
}

void Browser::new_tab(const Url &url) {
  auto tab = std::make_unique<Tab>(network_engine_, width_, *font_manager_);
  tab->load(url);
  tabs_.push_back(std::move(tab));
  active_tab_index_ = tabs_.size() - 1;
}

void Browser::switch_to_tab(size_t index) {
  if (index < tabs_.size()) {
    active_tab_index_ = index;
  }
}

void Browser::close_tab(size_t index) {
  if (index >= tabs_.size())
    return;

  tabs_.erase(tabs_.begin() + index);

  if (tabs_.empty()) {
    new_tab(Url("about:welcome"));
    return;
  }

  if (active_tab_index_ > index) {
    active_tab_index_--;
  } else if (active_tab_index_ == index) {
    if (active_tab_index_ >= tabs_.size()) {
      active_tab_index_ = tabs_.size() - 1;
    }
  }
}

Tab *Browser::active_tab() const {
  if (tabs_.empty())
    return nullptr;
  return tabs_[active_tab_index_].get();
}

void Browser::main_loop() {
  while (is_running_) {
    handle_events();
    if (!is_running_)
      break;
    draw();
    SDL_Delay(16); // Story: Target ~60 FPS
  }
}

void Browser::handle_events() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT)
      is_running_ = false;

    if (event.type == SDL_MOUSEBUTTONDOWN &&
        event.button.button == SDL_BUTTON_LEFT) {
      utils::Point point = {event.button.x, event.button.y};
      if (point.y < ui_.height()) {
        ui_.click(point);
      } else if (active_tab()) {
        active_tab()->handle_mousedown(point.x, point.y - ui_.height());
      }
    }

    if (event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT) {
      if (active_tab()) {
        active_tab()->handle_mouseup(event.button.x,
                                     event.button.y - ui_.height());
      }
    }

    if (event.type == SDL_MOUSEMOTION) {
      if (active_tab()) {
        active_tab()->handle_mousemove(event.button.x,
                                       event.button.y - ui_.height());
      }
    }

    if (event.type == SDL_KEYDOWN) {
      if (event.key.keysym.sym == SDLK_RETURN) {
        ui_.enter();
      } else {
        if (ui_.address_bar_focused()) {
          ui_.keypress(event.key.keysym.sym, "");
        } else if (active_tab()) {
          active_tab()->handle_keypress(event.key.keysym.sym, "");
        }
      }

      if (active_tab()) {
        if (event.key.keysym.sym == SDLK_DOWN)
          active_tab()->scroll_down();
        if (event.key.keysym.sym == SDLK_UP)
          active_tab()->scroll_up();
      }
    }

    if (event.type == SDL_MOUSEWHEEL && active_tab()) {
      if (event.wheel.y > 0)
        active_tab()->scroll_up();
      else if (event.wheel.y < 0)
        active_tab()->scroll_down();
    }

    if (event.type == SDL_TEXTINPUT) {
      if (ui_.address_bar_focused()) {
        ui_.keypress(0, event.text.text);
      } else if (active_tab()) {
        active_tab()->handle_keypress(0, event.text.text);
      }
    }
  }
}

void Browser::click(utils::Point point) {
  if (point.y < ui_.height()) {
    ui_.click(point);
  } else if (active_tab()) {
    active_tab()->click(point.x, point.y - ui_.height());
  }
}

void Browser::go_back() {
  if (active_tab())
    active_tab()->go_back();
}

void Browser::draw() {
  // Story: Clear the screen to a light gray for UI contrast
  SDL_SetRenderDrawColor(renderer_, 240, 240, 240, 255);
  SDL_RenderClear(renderer_);

  if (active_tab() && skia_ctx_) {
    skia_ctx_->clear(gfx::Color::White());

    // Story: Delegate rendering to the active tab's display list
    active_tab()->render(*skia_ctx_, ui_.height());

    if (!skia_texture_) {
      skia_texture_ =
          SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_ARGB8888,
                            SDL_TEXTUREACCESS_STREAMING, width_, height_);
    }

    void *pixels = skia_ctx_->get_pixels();
    int pitch = skia_ctx_->row_bytes();
    if (pixels) {
      SDL_UpdateTexture(skia_texture_, nullptr, pixels, pitch);
    }

    SDL_RenderCopy(renderer_, skia_texture_, nullptr, nullptr);
  }

  ui_.render(renderer_);
  SDL_RenderPresent(renderer_);
}

#include "Browser.h"
#include "request/HttpRequest.h"
#include <iostream>

Browser::Browser() : Browser(std::make_shared<HttpRequest>()) {}

Browser::Browser(std::shared_ptr<IRequest> http) : http_(std::move(http)), ui_(this) {
  initSDL();
  initTTF();
  loadFont();
  createWindow();
  createRenderer();
}

Browser::Browser::~Browser() { shutdown(); }

void Browser::initSDL() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    running = false;
  }
  SDL_StartTextInput(); // Enable for address bar
}

void Browser::initTTF() {
  if (TTF_Init() != 0) {
    std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
    running = false;
  }
}

void Browser::loadFont() {
  std::string font_path = std::string(ASSETS_DIR) + "/fonts/NotoSansCJK-Regular.ttc";
  font = TTF_OpenFont(font_path.c_str(), 16);
  if (!font) {
    std::cerr << "Font error: " << TTF_GetError() << std::endl;
    running = false;
  }
}

void Browser::createWindow() {
  window = SDL_CreateWindow("C Surfer 🌊", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "Window error: " << SDL_GetError() << std::endl;
    running = false;
  }
}

void Browser::createRenderer() {
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cerr << "Renderer error: " << SDL_GetError() << std::endl;
    running = false;
  }
}

void Browser::shutdown() {
  SDL_StopTextInput();
  if (renderer) SDL_DestroyRenderer(renderer);
  if (window) SDL_DestroyWindow(window);
  if (font) TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();
}

void Browser::load(const Url &url) {
  if (!running) return;

  if (tabs_.empty()) {
    new_tab(url);
  } else {
    active_tab()->load(url);
  }
}

void Browser::new_tab(const Url &url) {
  FontMetrics metrics{TTF_FontAscent(font), abs(TTF_FontDescent(font)), TTF_FontLineSkip(font)};
  auto tab = std::make_unique<Tab>(http_, WIDTH, metrics);
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
  if (index >= tabs_.size()) return;

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

Tab* Browser::active_tab() const {
  if (tabs_.empty()) return nullptr;
  return tabs_[active_tab_index_].get();
}

void Browser::mainLoop() {
  while (running) {
    handleEvents();
    if (!running) break; 
    draw();
    SDL_Delay(16);
  }
}

void Browser::handleEvents() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) running = false;

    if (e.type == SDL_MOUSEBUTTONDOWN) {
      if (e.button.button == SDL_BUTTON_LEFT) {
        if (e.button.y < ui_.height()) {
            ui_.click(e.button.x, e.button.y);
        } else if (active_tab()) {
            active_tab()->click(e.button.x, e.button.y - ui_.height());
        }
      }
    }

    if (e.type == SDL_KEYDOWN) {
      if (e.key.keysym.sym == SDLK_RETURN) {
        ui_.enter();
      } else {
        ui_.keypress(e.key.keysym.sym, "");
      }
      
      if (active_tab()) {
        if (e.key.keysym.sym == SDLK_DOWN) active_tab()->scrolldown();
        if (e.key.keysym.sym == SDLK_UP) active_tab()->scrollup();
      }
    }
    
    if (e.type == SDL_MOUSEWHEEL && active_tab()) {
      if (e.wheel.y > 0) active_tab()->scrollup();
      else if (e.wheel.y < 0) active_tab()->scrolldown();
    }
    
    if (e.type == SDL_TEXTINPUT) {
      ui_.keypress(0, e.text.text);
    }
  }
}

void Browser::click(int x, int y) {
  if (y < ui_.height()) {
    ui_.click(x, y);
  } else if (active_tab()) {
    active_tab()->click(x, y - ui_.height());
  }
}

void Browser::go_back() {
  if (active_tab()) active_tab()->go_back();
}

void Browser::draw() {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
  
  if (active_tab()) {
    active_tab()->render(renderer, ui_.height());
  }
  
  ui_.render(renderer);
  SDL_RenderPresent(renderer);
}




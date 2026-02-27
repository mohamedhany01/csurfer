#include "Browser.h"
#include "html/HTMLParser.h"
#include "layout/LayoutConstants.h"
#include "layout/LayoutTree.h"
#include "request/HttpRequest.h"
#include <algorithm>
#include <iostream>
#include <string>

Browser::Browser() : Browser(std::make_shared<HttpRequest>()) {}

Browser::Browser(std::shared_ptr<IRequest> http) : http_(std::move(http)) {

  initSDL();
  initTTF();
  loadFont();
  createWindow();
  createRenderer();
}

Browser::~Browser() { shutdown(); }

void Browser::initSDL() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    running = false;
  }
}

void Browser::initTTF() {
  if (TTF_Init() != 0) {
    std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
    running = false;
  }
}

void Browser::loadFont() {
  std::string font_path =
      std::string(ASSETS_DIR) + "/fonts/NotoSansCJK-Regular.ttc";

  font = TTF_OpenFont(font_path.c_str(), 16);
  if (!font) {
    std::cerr << "Font error: " << TTF_GetError() << std::endl;
    running = false;
  }
}

void Browser::createWindow() {
  window =
      SDL_CreateWindow("C Surfer 🌊", SDL_WINDOWPOS_CENTERED,
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
  if (renderer)
    SDL_DestroyRenderer(renderer);
  if (window)
    SDL_DestroyWindow(window);
  if (font)
    TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();
}

void Browser::load(const Url &url) {
  if (!running)
    return;

  // Fetch page body via request abstraction
  std::string body = http_->request(url);

  HTMLParser parser(body);
  root_ = parser.parse();
  if (!root_) {
    std::cout << "LOG :: no root found!" << std::endl;
    return;
  }

  HTMLParser::print_tree(*root_);

  // Extract font metrics
  FontMetrics metrics{TTF_FontAscent(font), abs(TTF_FontDescent(font)),
                      TTF_FontLineSkip(font)};

  // Build a layout tree for the parsed HTML and compute positions.
  document_ = std::make_unique<DocumentLayout>(root_.get(), metrics, WIDTH);
  document_->layout();

  display_list.clear();
  paint_tree(*document_, display_list);
  scroll = 0;

  // Start SDL loop
  mainLoop();
}

// Main SDL loop
void Browser::mainLoop() {
  while (running) {
    handleEvents();
    draw();
    SDL_Delay(16); // ~60 FPS
  }
}

void Browser::handleEvents() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT)
      running = false;

    if (e.type == SDL_KEYDOWN) {
      if (e.key.keysym.sym == SDLK_DOWN)
        scroll += 50;

      if (e.key.keysym.sym == SDLK_UP)
        scroll -= 50;

      int max_scroll = 0;
      if (document_) {
        max_scroll = std::max(document_->height + 2 * VSTEP - HEIGHT, 0);
      }
      scroll = std::clamp(scroll, 0, max_scroll);
    }
  }
}

void Browser::draw() {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
  for (const auto &cmd : display_list) {
    if (!cmd)
      continue;
    if (cmd->top > scroll + HEIGHT)
      continue;
    if (cmd->bottom < scroll)
      continue;
    cmd->execute(scroll, renderer);
  }
  SDL_RenderPresent(renderer);
}

#include "Browser.h"
#include "html/HTMLParser.h"
#include "layout/DisplayItem.h"
#include "layout/Layout.h"
#include "request/HttpRequest.h"
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

  // Extract text to build the display list with fixed coordinates (x, y)
  Layout layout(*root_, metrics, WIDTH - 40);
  display_list = layout.build();

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

      if (scroll < 0)
        scroll = 0;
    }
  }
}

void Browser::drawText(const DisplayItem &item) {
  SDL_Color color = {0, 0, 0, 255};

  SDL_Surface *surface =
      TTF_RenderUTF8_Blended(item.font, item.text.c_str(), color);

  if (!surface)
    return;

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_Rect dst{item.x, item.y, surface->w, surface->h};
  SDL_RenderCopy(renderer, texture, nullptr, &dst);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void Browser::draw() {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
  for (const auto &item : display_list) {

    // new position on screen, item.y is always fixed only screen_y is
    // changeable
    int screen_y = item.y - scroll;

    // don't draw overflowed text in the viewport (optimization)
    if (screen_y < -20 || screen_y > HEIGHT + 20)
      continue;

    DisplayItem new_di{item.x, screen_y, item.text, item.font};

    drawText(new_di);
  }
  SDL_RenderPresent(renderer);
}

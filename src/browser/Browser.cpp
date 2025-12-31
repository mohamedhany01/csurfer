#include "Browser.h"
#include "request/HttpRequest.h"
#include "utils/Parser.h"
#include <iostream>
#include <string>

Browser::Browser() : Browser(std::make_shared<HttpRequest>()) {}

Browser::Browser(std::shared_ptr<IRequest> http) : http_(std::move(http)) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    running = false;
  }

  if (TTF_Init() != 0) {
    std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
    running = false;
  }

  std::string font_path =
      std::string(ASSETS_DIR) + "/fonts/NotoSansCJK-Regular.ttc";
  font = TTF_OpenFont(font_path.c_str(), 16);

  if (!font) {
    std::cerr << "Font error: " << TTF_GetError() << std::endl;
    running = false;
  }

  window =
      SDL_CreateWindow("C Surfer 🌊", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "Window error: " << SDL_GetError() << std::endl;
    running = false;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cerr << "Renderer error: " << SDL_GetError() << std::endl;
    running = false;
  }
}

Browser::~Browser() {
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
  page_text = utils::lex(body);

  buildDisplayList();

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

void Browser::buildDisplayList() {
  display_list.clear();

  FontMetrics metrics{TTF_FontAscent(font), abs(TTF_FontDescent(font)),
                      TTF_FontLineSkip(font)};

  int cursor_x = 16;
  int cursor_y = 18 + metrics.ascent;

  const int max_width = WIDTH - 40; // width with some padding

  std::vector<std::string> words = utils::splitWords(page_text);

  for (const std::string &word : words) {

    if (word == "\n") {
      cursor_y += metrics.lineSkip * 1.25;
      cursor_x = 20;
      continue;
    }

    int w, h;
    TTF_SizeUTF8(
        font, word.c_str(), &w,
        &h); // measure rendered text size (in pixels) using current font
    // wrap line
    if (cursor_x + w > max_width) {
      cursor_y += metrics.lineSkip * 1.25;
      cursor_x = 20;
    }

    display_list.push_back({cursor_x, cursor_y, word});

    // advance cursor (add space width)
    int space_w;
    TTF_SizeUTF8(
        font, " ", &space_w,
        nullptr); // measure rendered text size (in pixels) using current font

    cursor_x += w + space_w;
  }
}

void Browser::drawText(const std::string &glyph, int cursor_x, int cursor_y) {
  SDL_Color color = {0, 0, 0, 255};

  SDL_Surface *surface = TTF_RenderUTF8_Blended(font, glyph.c_str(), color);

  if (!surface)
    return;

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_Rect dst{cursor_x, cursor_y, surface->w, surface->h};
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

    drawText(item.glyph, item.x, screen_y);
  }
  SDL_RenderPresent(renderer);
}

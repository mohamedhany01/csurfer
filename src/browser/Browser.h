#pragma once
#include "request/IRequest.h"
#include "url/Url.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>
#include <vector>

struct DisplayItem {
  const int x;             // page coordinate
  const int y;             // page coordinate
  const std::string glyph; // UTF-8
};

class Browser {
public:
  Browser();
  explicit Browser(std::shared_ptr<IRequest> http);
  ~Browser();

  void load(const Url &url); // Load page and start SDL loop

private:
  // Request
  std::shared_ptr<IRequest> http_;

  // SDL2
  int WIDTH = 800;
  int HEIGHT = 600;
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  bool running = true;
  TTF_Font *font = nullptr;

  std::string page_text;

  // SDL2 loop
  void mainLoop();
  void handleEvents();
  void draw();
  void buildDisplayList();
  void drawText(const std::string &glyph, int cursor_x, int cursor_y);

  // layout/scrolling
  std::vector<DisplayItem> display_list;
  int scroll = 0;
};

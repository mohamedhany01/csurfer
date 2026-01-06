#pragma once
#include "layout/DisplayItem.h"
#include "lexer/Lexeme.h"
#include "request/IRequest.h"
#include "url/Url.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>
#include <vector>
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
  const int WIDTH = 800;
  const int HEIGHT = 600;
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  bool running = true;
  // TODO: maybe need to move this to layout/layout.cpp
  TTF_Font *font = nullptr;

  // SDL2 core
  void initSDL();
  void initTTF();
  void loadFont();
  void createWindow();
  void createRenderer();
  void shutdown();

  // SDL2 loop
  void mainLoop();
  void handleEvents();
  void draw();
  void drawText(const DisplayItem &item);

  // layout/scrolling
  std::vector<DisplayItem> display_list;
  int scroll = 0;

  // Lexer
  std::vector<std::unique_ptr<Lexeme>> tokens_;
};

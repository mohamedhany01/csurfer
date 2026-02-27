#pragma once
#include "layout/DisplayItem.h"
#include "layout/DocumentLayout.h"
#include "lexer/Element.h"
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

  // Load a URL, build the layout tree, and start the SDL render loop.
  //
  // Example:
  //   Browser browser;
  //   browser.load(Url("http://localhost:8000/index.html"));
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

  // layout/scrolling
  std::unique_ptr<DocumentLayout> document_;
  std::vector<std::unique_ptr<DrawCommand>> display_list;
  int scroll = 0;

  // Lexer
  std::unique_ptr<Element> root_;
};

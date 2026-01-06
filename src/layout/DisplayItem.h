#pragma once
#include <SDL_ttf.h>
#include <string>

struct DisplayItem {
  const int x;      // page coordinate
  const int y;      // page coordinate
  std::string text; // UTF-8
  TTF_Font *font;   // font data
};

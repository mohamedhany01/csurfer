#include "layout/DisplayItem.h"

DrawText::DrawText(int x1, int y1, std::string text, TTF_Font *font)
    : text_(std::move(text)), font_(font) {
  left = x1;
  top = y1;
  right = x1;
  bottom = y1 + (font_ ? TTF_FontLineSkip(font_) : 0);
}

void DrawText::execute(int scroll, SDL_Renderer *renderer) const {
  if (!renderer || !font_)
    return;

  SDL_Color color{0, 0, 0, 255};

  SDL_Surface *surface = TTF_RenderUTF8_Blended(font_, text_.c_str(), color);
  if (!surface)
    return;

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    SDL_FreeSurface(surface);
    return;
  }

  SDL_Rect dst{left, top - scroll, surface->w, surface->h};
  SDL_RenderCopy(renderer, texture, nullptr, &dst);

  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
}

DrawRect::DrawRect(int x1, int y1, int x2, int y2, SDL_Color color)
    : color_(color) {
  left = x1;
  top = y1;
  right = x2;
  bottom = y2;
}

void DrawRect::execute(int scroll, SDL_Renderer *renderer) const {
  if (!renderer)
    return;

  SDL_SetRenderDrawColor(renderer, color_.r, color_.g, color_.b, color_.a);
  SDL_Rect rect{left, top - scroll, right - left, bottom - top};
  SDL_RenderFillRect(renderer, &rect);
}

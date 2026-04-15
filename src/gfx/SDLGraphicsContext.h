#pragma once
#include "GraphicsContext.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

namespace gfx {

/**
 * SDLGraphicsContext is a temporary adapter that allows CSurfer to use
 * the GraphicsContext interface while still rendering via SDL_Renderer.
 * This facilitates the step-by-step migration to Skia.
 */
class SDLGraphicsContext : public GraphicsContext {
public:
  explicit SDLGraphicsContext(SDL_Renderer *renderer) : renderer_(renderer) {}

  /**
   * Draws a filled rectangle using SDL_RenderFillRect.
   */
  void draw_rect(const Rect &rect, const Color &color) override {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    SDL_Rect r{rect.x, rect.y, rect.width, rect.height};
    SDL_RenderFillRect(renderer_, &r);
  }

  /**
   * Draws a line. Supports basic thickness by drawing multiple lines.
   */
  void draw_line(int x1, int y1, int x2, int y2, const Color &color,
                 int thickness = 1) override {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    for (int i = 0; i < thickness; ++i) {
      SDL_RenderDrawLine(renderer_, x1 + i, y1, x2 + i, y2);
    }
  }

  /**
   * Renders text using SDL_ttf.
   * Note: This is expensive in SDL as it creates a surface and texture every
   * frame.
   */
  void draw_text(int x, int y, const std::string &text, const Color &color,
                 void *font_handle) override {
    if (!font_handle)
      return;
    TTF_Font *font = static_cast<TTF_Font *>(font_handle);
    SDL_Color sdl_color{color.r, color.g, color.b, color.a};
    SDL_Surface *surface =
        TTF_RenderUTF8_Blended(font, text.c_str(), sdl_color);
    if (!surface)
      return;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer_, surface);
    if (texture) {
      SDL_Rect dst{x, y, surface->w, surface->h};
      SDL_RenderCopy(renderer_, texture, nullptr, &dst);
      SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
  }

  /**
   * SDL fallback for rounded rectangles.
   */
  void draw_rounded_rect(const Rect &rect, float radius,
                         const Color &color) override {
    // SDL_Renderer doesn't support rounded rects natively without extra
    // geometry.
    draw_rect(rect, color);
  }

  void save_layer(float opacity) override {
    // Not supported in basic SDL_Renderer without SDL_Texture target switching.
  }

  void restore() override {
    // Not supported.
  }

  void clip_rect(const Rect &rect) override {
    // SDL_RenderSetClipRect could be used here if needed.
  }

  /**
   * Clears the current rendering target.
   */
  void clear(const Color &color) override {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer_);
  }

private:
  SDL_Renderer *renderer_;
};

} // namespace gfx

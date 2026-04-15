#include "layout/DisplayItem.h"
#include "gfx/GraphicsContext.h"

DrawText::DrawText(int x1, int y1, std::string text, TTF_Font *font,
                   SDL_Color color)
    : text_(std::move(text)), font_(font), color_(color) {
  left = x1;
  top = y1;
  right = x1;
  bottom = y1 + (font_ ? TTF_FontLineSkip(font_) : 0);
}

/**
 * Stage 1.1: DrawText now uses the GraphicsContext abstraction.
 * It passes the raw font handle for backward compatibility with SDL.
 */
void DrawText::execute(int scroll, int y_offset,
                       gfx::GraphicsContext &ctx) const {
  ctx.draw_text(left, top - scroll + y_offset, text_,
                gfx::Color::FromRGBA(color_.r, color_.g, color_.b, color_.a),
                font_);
}

DrawRect::DrawRect(int x1, int y1, int x2, int y2, SDL_Color color)
    : color_(color) {
  left = x1;
  top = y1;
  right = x2;
  bottom = y2;
}

/**
 * Stage 1.1: DrawRect now uses ctx.draw_rect.
 */
void DrawRect::execute(int scroll, int y_offset,
                       gfx::GraphicsContext &ctx) const {
  ctx.draw_rect({left, top - scroll + y_offset, right - left, bottom - top},
                gfx::Color::FromRGBA(color_.r, color_.g, color_.b, color_.a));
}

DrawLine::DrawLine(int x1, int y1, int x2, int y2, SDL_Color color,
                   int thickness)
    : color_(color), thickness_(thickness) {
  left = x1;
  top = y1;
  right = x2;
  bottom = y2;
}

/**
 * Stage 1.1: DrawLine now uses ctx.draw_line.
 */
void DrawLine::execute(int scroll, int y_offset,
                       gfx::GraphicsContext &ctx) const {
  ctx.draw_line(
      left, top - scroll + y_offset, right, bottom - scroll + y_offset,
      gfx::Color::FromRGBA(color_.r, color_.g, color_.b, color_.a), thickness_);
}

#include "layout/DisplayItem.h"
#include "gfx/GraphicsContext.h"

DrawText::DrawText(int x1, int y1, std::string text,
                   std::shared_ptr<gfx::Font> font, gfx::Color color)
    : text_(std::move(text)), font_(std::move(font)), color_(color) {
  left = x1;
  top = y1;
  right = x1;
  // Note: bottom is usually calculated from font metrics, but for now we
  // initialize it to top. The actual bounding box is updated during layout.
  bottom = y1;
}

/**
 * Stage 1.2: DrawText now uses the generic GraphicsContext.
 * It passes the opaque font handle.
 */
void DrawText::execute(int scroll, int y_offset,
                       gfx::GraphicsContext &ctx) const {
  ctx.draw_text(left, top - scroll + y_offset, text_, color_, font_);
}

DrawRect::DrawRect(int x1, int y1, int x2, int y2, gfx::Color color)
    : color_(color) {
  left = x1;
  top = y1;
  right = x2;
  bottom = y2;
}

/**
 * Stage 1.2: DrawRect now uses ctx.draw_rect with gfx::Color.
 */
void DrawRect::execute(int scroll, int y_offset,
                       gfx::GraphicsContext &ctx) const {
  ctx.draw_rect({left, top - scroll + y_offset, right - left, bottom - top},
                color_);
}

void DrawRoundedRect::execute(int scroll, int y_offset,
                              gfx::GraphicsContext &ctx) const {
  ctx.draw_rounded_rect(
      {rect_.x, rect_.y - scroll + y_offset, rect_.width, rect_.height},
      radius_, color_);
}

DrawLine::DrawLine(int x1, int y1, int x2, int y2, gfx::Color color,
                   int thickness)
    : color_(color), thickness_(thickness) {
  left = x1;
  top = y1;
  right = x2;
  bottom = y2;
}

/**
 * Stage 1.2: DrawLine now uses ctx.draw_line with gfx::Color.
 */
void DrawLine::execute(int scroll, int y_offset,
                       gfx::GraphicsContext &ctx) const {
  ctx.draw_line(left, top - scroll + y_offset, right,
                bottom - scroll + y_offset, color_, thickness_);
}

void DrawSaveLayer::execute(int /*scroll*/, int /*y_offset*/,
                            gfx::GraphicsContext &ctx) const {
  ctx.save_layer(opacity_);
}

void DrawRestore::execute(int /*scroll*/, int /*y_offset*/,
                          gfx::GraphicsContext &ctx) const {
  ctx.restore();
}

DrawBoxShadow::DrawBoxShadow(const Rect &rect, float radius, int dx, int dy,
                             gfx::Color color)
    : rect_(rect), radius_(radius), dx_(dx), dy_(dy), color_(color) {
  left = rect.x + std::min(0, dx) - (int)radius;
  top = rect.y + std::min(0, dy) - (int)radius;
  right = rect.x + rect.width + std::max(0, dx) + (int)radius;
  bottom = rect.y + rect.height + std::max(0, dy) + (int)radius;
}

void DrawBoxShadow::execute(int scroll, int y_offset,
                            gfx::GraphicsContext &ctx) const {
  ctx.draw_box_shadow(
      {rect_.x, rect_.y - scroll + y_offset, rect_.width, rect_.height},
      radius_, dx_, dy_, color_);
}

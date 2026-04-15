#include "SkiaContext.h"
#include <core/SkColor.h>
#include <core/SkPath.h>
#include <core/SkRRect.h>

namespace gfx {

static SkColor ToSkColor(const Color &c) {
  return SkColorSetARGB(c.a, c.r, c.g, c.b);
}

SkiaContext::SkiaContext(int width, int height)
    : width_(width), height_(height) {
  surface_ = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(width, height));
  if (surface_) {
    canvas_ = surface_->getCanvas();
    canvas_->drawColor(SK_ColorWHITE);
  }
}

void SkiaContext::draw_rect(const Rect &rect, const Color &color) {
  if (!canvas_)
    return;
  paint_.setColor(ToSkColor(color));
  paint_.setStyle(SkPaint::kFill_Style);
  canvas_->drawRect(SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height),
                    paint_);
}

void SkiaContext::draw_line(int x1, int y1, int x2, int y2, const Color &color,
                            int thickness) {
  if (!canvas_)
    return;
  paint_.setColor(ToSkColor(color));
  paint_.setStrokeWidth(thickness);
  paint_.setStyle(SkPaint::kStroke_Style);
  canvas_->drawLine(x1, y1, x2, y2, paint_);
}

void SkiaContext::draw_text(int x, int y, const std::string &text,
                            const Color &color, void *font_handle) {
  if (!canvas_)
    return;
  paint_.setColor(ToSkColor(color));
  paint_.setStyle(SkPaint::kFill_Style);

  // For now, using a default system font of size 16
  SkFont font(nullptr, 16);
  canvas_->drawString(text.c_str(), x, y, font, paint_);
}

void SkiaContext::draw_rounded_rect(const Rect &rect, float radius,
                                    const Color &color) {
  if (!canvas_)
    return;
  paint_.setColor(ToSkColor(color));
  paint_.setStyle(SkPaint::kFill_Style);
  SkRRect rrect;
  rrect.setRectXY(SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height),
                  radius, radius);
  canvas_->drawRRect(rrect, paint_);
}

void SkiaContext::save_layer(float opacity) {
  if (!canvas_)
    return;
  SkPaint layer_paint;
  layer_paint.setAlphaf(opacity);
  canvas_->saveLayer(nullptr, &layer_paint);
}

void SkiaContext::restore() {
  if (canvas_)
    canvas_->restore();
}

void SkiaContext::clip_rect(const Rect &rect) {
  if (!canvas_)
    return;
  canvas_->clipRect(SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height),
                    SkClipOp::kIntersect, true);
}

void SkiaContext::clear(const Color &color) {
  if (canvas_)
    canvas_->drawColor(ToSkColor(color));
}

void *SkiaContext::get_pixels() const {
  if (!surface_)
    return nullptr;
  SkPixmap pixmap;
  if (surface_->peekPixels(&pixmap)) {
    return (void *)pixmap.addr();
  }
  return nullptr;
}

int SkiaContext::row_bytes() const {
  if (!surface_)
    return 0;
  SkPixmap pixmap;
  if (surface_->peekPixels(&pixmap)) {
    return (int)pixmap.rowBytes();
  }
  return 0;
}

} // namespace gfx

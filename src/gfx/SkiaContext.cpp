#include "SkiaContext.h"
#include "SkiaFont.h"
#include <core/SkColor.h>
#include <core/SkFontMetrics.h>
#include <core/SkFontMgr.h>
#include <core/SkPath.h>
#include <core/SkRRect.h>
#include <ports/SkFontMgr_directory.h>

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

  // Fallback typeface if no font is provided during draw_text
  std::string fonts_dir = std::string(ASSETS_DIR) + "/fonts";
  sk_sp<SkFontMgr> font_mgr = SkFontMgr_New_Custom_Directory(fonts_dir.c_str());

  if (font_mgr) {
    std::string font_path = fonts_dir + "/Ubuntu-Regular.ttf";
    typeface_ = font_mgr->makeFromFile(font_path.c_str());
  }

  if (!typeface_) {
    typeface_ = SkTypeface::MakeEmpty(); // Last resort
  }

  paint_.setAntiAlias(true);
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
                            const Color &color, std::shared_ptr<Font> font) {
  if (!canvas_)
    return;
  paint_.setColor(ToSkColor(color));
  paint_.setStyle(SkPaint::kFill_Style);

  SkFont sk_font;
  if (font) {
    auto skia_font = std::static_pointer_cast<SkiaFont>(font);
    sk_font = skia_font->sk_font();
  } else {
    // Fallback to internal typeface if no font provided
    sk_font = SkFont(typeface_, 16);
  }

  // Baseline Correction
  // Skia's y is the baseline. Our layout y is the top.
  // Shifting downward by the font's ascent correctly aligns the two.
  SkFontMetrics metrics;
  sk_font.getMetrics(&metrics);
  float y_baseline = y + std::abs(metrics.fAscent);

  canvas_->drawString(text.c_str(), x, y_baseline, sk_font, paint_);
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

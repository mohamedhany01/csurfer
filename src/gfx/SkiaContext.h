#pragma once
#include "GraphicsContext.h"
#include <core/SkBitmap.h>
#include <core/SkCanvas.h>
#include <core/SkFont.h>
#include <core/SkPaint.h>
#include <core/SkSurface.h>
#include <core/SkTypeface.h>
#include <memory>

namespace gfx {

/**
 * Stage 2.1.4: Refactored to use SkSurface for better pixel buffering and
 * rendering correctness. This is the implementation of Section One of Chapter
 * 11.
 */
class SkiaContext final : public GraphicsContext {
public:
  SkiaContext(int width, int height);
  ~SkiaContext() override = default;

  // Implementation of GraphicsContext
  void draw_rect(const Rect &rect, const Color &color) override;
  void draw_line(int x1, int y1, int x2, int y2, const Color &color,
                 int thickness = 1) override;
  void draw_text(int x, int y, const std::string &text, const Color &color,
                 std::shared_ptr<Font> font = nullptr) override;
  void draw_rounded_rect(const Rect &rect, float radius,
                         const Color &color) override;

  void draw_box_shadow(const Rect &rect, float radius, int dx, int dy,
                       const Color &color) override;

  void draw_linear_gradient(const Rect &rect, const Color &color1,
                            const Color &color2,
                            const std::string &direction) override;

  void save_layer(float opacity) override;
  void restore() override;
  void clip_rect(const Rect &rect) override;
  void clear(const Color &color) override;

  // Pixel Access for SDL
  void *get_pixels() const;
  int row_bytes() const;

private:
  int width_;
  int height_;

  sk_sp<SkSurface> surface_;
  SkCanvas *canvas_ = nullptr;
  sk_sp<SkTypeface> typeface_;

  // Cached SkPaint to avoid re-allocation
  SkPaint paint_;
};

} // namespace gfx

#pragma once
#include "GraphicsContext.h"
#include <core/SkBitmap.h>
#include <core/SkCanvas.h>
#include <core/SkFont.h>
#include <core/SkPaint.h>
#include <core/SkSurface.h>

namespace gfx {

class SkiaContext final : public GraphicsContext {
public:
  SkiaContext(int width, int height);
  ~SkiaContext() override = default;

  // Implementation of GraphicsContext
  void draw_rect(const Rect &rect, const Color &color) override;
  void draw_line(int x1, int y1, int x2, int y2, const Color &color,
                 int thickness = 1) override;
  void draw_text(int x, int y, const std::string &text, const Color &color,
                 void *font_handle = nullptr) override;
  void draw_rounded_rect(const Rect &rect, float radius,
                         const Color &color) override;
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

  SkBitmap bitmap_;
  std::unique_ptr<SkCanvas> canvas_;

  // Cached SkPaint to avoid re-allocation
  SkPaint paint_;
};

} // namespace gfx

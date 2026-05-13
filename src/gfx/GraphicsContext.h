#pragma once
#include "Color.h"
#include "Font.h"
#include "layout/Rect.h"
#include <string>
#include <memory>

namespace gfx {

/**
 * GraphicsContext is an abstract interface for all drawing operations.
 * It decouples the browser's layout engine from the rendering backend
 * (Skia/SDL).
 *
 * Theory: This allows us to "record" drawing commands and manage "Stacking
 * Contexts" independently of the layout process.
 */
class GraphicsContext {
public:
  virtual ~GraphicsContext() = default;

  // Basic Primitives
  virtual void draw_rect(const Rect &rect, const Color &color) = 0;
  virtual void draw_line(int x1, int y1, int x2, int y2, const Color &color,
                         int thickness = 1) = 0;

  // Text Rendering (Using a simplified font abstraction)
  virtual void draw_text(int x, int y, const std::string &text,
                         const Color &color, std::shared_ptr<Font> font = nullptr) = 0;

  // Rounded Rectangles
  virtual void draw_rounded_rect(const Rect &rect, float radius,
                                 const Color &color) = 0;

  // Stacking Contexts & Visual Effects
  //
  // Theory: save_layer allocates a new temporary surface for transparency.
  // This is expensive because it triggers a new memory allocation and a
  // blend pass.
  virtual void save_layer(float opacity) = 0;

  // Theory: restore collapses the current layer back into its parent.
  virtual void restore() = 0;

  // Theory: clipping limits all future drawing to the given bounds.
  // Essential for 'overflow: hidden' or 'overflow: clip'.
  virtual void clip_rect(const Rect &rect) = 0;

  // Canvas Management
  virtual void clear(const Color &color) = 0;
};

} // namespace gfx

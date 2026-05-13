#pragma once
#include "gfx/Color.h"
#include "gfx/Font.h"
#include "layout/Rect.h"
#include <memory>
#include <string>

namespace gfx {
class GraphicsContext;
}

/**
 * A single drawing command in the final display list.
 *
 * Coordinates are in page space (not yet scrolled). The execute method takes
 * the current scroll offset and a GraphicsContext to actually draw.
 */
class DrawCommand {
public:
  virtual ~DrawCommand() = default;

  int top = 0;
  int left = 0;
  int bottom = 0;
  int right = 0;

  /**
   * Draw this command using the given scroll offset and graphics context.
   * y_offset is used to shift content below the CSurfer UI.
   */
  virtual void execute(int scroll, int y_offset,
                       gfx::GraphicsContext &ctx) const = 0;
};

/**
 * Draw a single piece of text at a fixed page position.
 *
 * Stage 1.2: Removed SDL-specific TTF_Font and SDL_Color.
 */
class DrawText final : public DrawCommand {
public:
  DrawText(int x1, int y1, std::string text, std::shared_ptr<gfx::Font> font,
           gfx::Color color = gfx::Color::Black());

  void execute(int scroll, int y_offset,
               gfx::GraphicsContext &ctx) const override;

private:
  std::string text_;
  std::shared_ptr<gfx::Font> font_;
  gfx::Color color_;
};

/**
 * Draw a filled rectangle, used for things like code block backgrounds.
 */
class DrawRect final : public DrawCommand {
public:
  DrawRect(int x1, int y1, int x2, int y2, gfx::Color color);

  void execute(int scroll, int y_offset,
               gfx::GraphicsContext &ctx) const override;

private:
  gfx::Color color_ = gfx::Color::Black();
};

/**
 * Draw a line between two points. Useful for borders and carets.
 */
class DrawRoundedRect final : public DrawCommand {
public:
  DrawRoundedRect(const Rect &rect, float radius, const gfx::Color &color)
      : rect_(rect), radius_(radius), color_(color) {}
  void execute(int scroll, int y_offset,
               gfx::GraphicsContext &ctx) const override;

private:
  Rect rect_;
  float radius_;
  gfx::Color color_;
};

class DrawLine final : public DrawCommand {
public:
  DrawLine(int x1, int y1, int x2, int y2, gfx::Color color, int thickness = 1);

  void execute(int scroll, int y_offset,
               gfx::GraphicsContext &ctx) const override;

private:
  gfx::Color color_;
  int thickness_;
};

/**
 * Pushes a new transparent layer onto the graphics context.
 * All subsequent drawing operations will be composited into this layer
 * until DrawRestore is executed.
 */
class DrawSaveLayer final : public DrawCommand {
public:
  explicit DrawSaveLayer(float opacity) : opacity_(opacity) {}

  void execute(int scroll, int y_offset,
               gfx::GraphicsContext &ctx) const override;

private:
  float opacity_;
};

/**
 * Restores the graphics context, compositing the previously saved layer
 * to the screen with its specified opacity.
 */
class DrawRestore final : public DrawCommand {
public:
  void execute(int scroll, int y_offset,
               gfx::GraphicsContext &ctx) const override;
};

/**
 * Draws a drop shadow for a rectangle.
 */
class DrawBoxShadow final : public DrawCommand {
public:
  DrawBoxShadow(const Rect &rect, float radius, int dx, int dy,
                gfx::Color color);

  void execute(int scroll, int y_offset,
               gfx::GraphicsContext &ctx) const override;

private:
  Rect rect_;
  float radius_;
  int dx_, dy_;
  gfx::Color color_;
};

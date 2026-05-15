#pragma once
#include "gfx/Color.h"
#include "gfx/Font.h"
#include "utils/Geometry.h"
#include <memory>
#include <string>

namespace gfx {
class GraphicsContext;
}

/**
 * Story: A single drawing command in the final display list.
 *
 * Use-case: Coordinates are in page space (not yet scrolled). The execute
 * method takes the current scroll offset and a GraphicsContext to draw.
 */
class DrawCommand {
public:
  virtual ~DrawCommand() = default;

  /**
   * Story: Draws this command using the given scroll offset and context.
   * y_offset: Shifts content below the browser's UI bar.
   */
  virtual void execute(int scroll_offset, int y_screen_offset,
                       gfx::GraphicsContext &graphics_context) const = 0;

  // Clipping/Invalidation hints (can be used for optimization)
  int top = 0;
  int left = 0;
  int bottom = 0;
  int right = 0;
};

/**
 * Story: Draws a single piece of text.
 */
class DrawText final : public DrawCommand {
public:
  DrawText(int x_pos, int y_pos, std::string content,
           std::shared_ptr<gfx::Font> font_handle,
           gfx::Color text_color = gfx::Color::Black());

  void execute(int scroll_offset, int y_screen_offset,
               gfx::GraphicsContext &graphics_context) const override;

private:
  int x_, y_;
  std::string content_;
  std::shared_ptr<gfx::Font> font_;
  gfx::Color color_;
};

/**
 * Story: Draws a solid filled rectangle.
 */
class DrawRect final : public DrawCommand {
public:
  DrawRect(int x1, int y1, int x2, int y2, gfx::Color fill_color);

  void execute(int scroll_offset, int y_screen_offset,
               gfx::GraphicsContext &graphics_context) const override;

private:
  int x1_, y1_, x2_, y2_;
  gfx::Color color_ = gfx::Color::Black();
};

/**
 * Story: Draws a filled rectangle with rounded corners.
 */
class DrawRoundedRect final : public DrawCommand {
public:
  DrawRoundedRect(const utils::Rect &bounds, float corner_radius,
                  const gfx::Color &fill_color)
      : bounds_(bounds), radius_(corner_radius), color_(fill_color) {}

  void execute(int scroll_offset, int y_screen_offset,
               gfx::GraphicsContext &graphics_context) const override;

private:
  utils::Rect bounds_;
  float radius_;
  gfx::Color color_;
};

/**
 * Story: Draws a straight line.
 */
class DrawLine final : public DrawCommand {
public:
  DrawLine(int x1, int y1, int x2, int y2, gfx::Color line_color,
           int thickness = 1);

  void execute(int scroll_offset, int y_screen_offset,
               gfx::GraphicsContext &graphics_context) const override;

private:
  int x1_, y1_, x2_, y2_;
  gfx::Color color_;
  int thickness_;
};

/**
 * Story: Pushes a new graphics layer for effects like opacity.
 */
class DrawSaveLayer final : public DrawCommand {
public:
  explicit DrawSaveLayer(float opacity_value, std::string blending_mode = "")
      : opacity_(opacity_value), blend_mode_(std::move(blending_mode)) {}

  void execute(int scroll_offset, int y_screen_offset,
               gfx::GraphicsContext &graphics_context) const override;

private:
  float opacity_;
  std::string blend_mode_;
};

/**
 * Story: Restores the graphics state and composites the previous layer.
 */
class DrawRestore final : public DrawCommand {
public:
  void execute(int scroll_offset, int y_screen_offset,
               gfx::GraphicsContext &graphics_context) const override;
};

/**
 * Story: Draws a drop shadow for a box.
 */
class DrawBoxShadow final : public DrawCommand {
public:
  DrawBoxShadow(const utils::Rect &bounds, float blur_radius, int dx, int dy,
                gfx::Color shadow_color);

  void execute(int scroll_offset, int y_screen_offset,
               gfx::GraphicsContext &graphics_context) const override;

private:
  utils::Rect bounds_;
  float radius_;
  int dx_, dy_;
  gfx::Color color_;
};

/**
 * Story: Draws a linear color gradient.
 */
class DrawLinearGradient final : public DrawCommand {
public:
  DrawLinearGradient(const utils::Rect &bounds, gfx::Color start_color,
                     gfx::Color end_color, std::string gradient_direction);

  void execute(int scroll_offset, int y_screen_offset,
               gfx::GraphicsContext &graphics_context) const override;

private:
  utils::Rect bounds_;
  gfx::Color color1_, color2_;
  std::string direction_;
};

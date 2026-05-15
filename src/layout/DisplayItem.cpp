#include "layout/DisplayItem.h"
#include "gfx/GraphicsContext.h"

DrawText::DrawText(int x_position, int y_position, std::string content,
                   std::shared_ptr<gfx::Font> font_handle,
                   gfx::Color text_color)
    : x_position_(x_position), y_position_(y_position),
      content_(std::move(content)), font_(std::move(font_handle)),
      color_(text_color) {
  left = x_position;
  top = y_position;
  right = x_position;
  bottom = y_position;
}

void DrawText::execute(int scroll_offset, int y_screen_offset,
                       gfx::GraphicsContext &graphics_context) const {
  graphics_context.draw_text(x_position_,
                             y_position_ - scroll_offset + y_screen_offset,
                             content_, color_, font_);
}

DrawRect::DrawRect(int start_x, int start_y, int end_x, int end_y,
                   gfx::Color fill_color)
    : start_x_(start_x), start_y_(start_y), end_x_(end_x), end_y_(end_y),
      color_(fill_color) {
  left = start_x;
  top = start_y;
  right = end_x;
  bottom = end_y;
}

void DrawRect::execute(int scroll_offset, int y_screen_offset,
                       gfx::GraphicsContext &graphics_context) const {
  graphics_context.draw_rect(
      {{start_x_, start_y_ - scroll_offset + y_screen_offset},
       end_x_ - start_x_,
       end_y_ - start_y_},
      color_);
}

void DrawRoundedRect::execute(int scroll_offset, int y_screen_offset,
                              gfx::GraphicsContext &graphics_context) const {
  graphics_context.draw_rounded_rect(
      {{bounds_.origin.x, bounds_.origin.y - scroll_offset + y_screen_offset},
       bounds_.width,
       bounds_.height},
      radius_, color_);
}

DrawLine::DrawLine(int start_x, int start_y, int end_x, int end_y,
                   gfx::Color line_color, int line_thickness)
    : start_x_(start_x), start_y_(start_y), end_x_(end_x), end_y_(end_y),
      color_(line_color), thickness_(line_thickness) {
  left = start_x;
  top = start_y;
  right = end_x;
  bottom = end_y;
}

void DrawLine::execute(int scroll_offset, int y_screen_offset,
                       gfx::GraphicsContext &graphics_context) const {
  graphics_context.draw_line(
      start_x_, start_y_ - scroll_offset + y_screen_offset, end_x_,
      end_y_ - scroll_offset + y_screen_offset, color_, thickness_);
}

void DrawSaveLayer::execute(int /*scroll_offset*/, int /*y_screen_offset*/,
                            gfx::GraphicsContext &graphics_context) const {
  graphics_context.save_layer(opacity_, blend_mode_);
}

void DrawRestore::execute(int /*scroll_offset*/, int /*y_screen_offset*/,
                          gfx::GraphicsContext &graphics_context) const {
  graphics_context.restore();
}

DrawBoxShadow::DrawBoxShadow(const utils::Rect &bounds, float blur_radius,
                             int offset_x, int offset_y,
                             gfx::Color shadow_color)
    : bounds_(bounds), radius_(blur_radius), offset_x_(offset_x),
      offset_y_(offset_y), color_(shadow_color) {
  left = bounds.origin.x + std::min(0, offset_x) - (int)blur_radius;
  top = bounds.origin.y + std::min(0, offset_y) - (int)blur_radius;
  right =
      bounds.origin.x + bounds.width + std::max(0, offset_x) + (int)blur_radius;
  bottom = bounds.origin.y + bounds.height + std::max(0, offset_y) +
           (int)blur_radius;
}

void DrawBoxShadow::execute(int scroll_offset, int y_screen_offset,
                            gfx::GraphicsContext &graphics_context) const {
  graphics_context.draw_box_shadow(
      {{bounds_.origin.x, bounds_.origin.y - scroll_offset + y_screen_offset},
       bounds_.width,
       bounds_.height},
      radius_, offset_x_, offset_y_, color_);
}

DrawLinearGradient::DrawLinearGradient(const utils::Rect &bounds,
                                       gfx::Color start_color,
                                       gfx::Color end_color,
                                       std::string gradient_direction)
    : bounds_(bounds), color1_(start_color), color2_(end_color),
      direction_(std::move(gradient_direction)) {
  left = bounds.origin.x;
  top = bounds.origin.y;
  right = bounds.origin.x + bounds.width;
  bottom = bounds.origin.y + bounds.height;
}

void DrawLinearGradient::execute(int scroll_offset, int y_screen_offset,
                                 gfx::GraphicsContext &graphics_context) const {
  graphics_context.draw_linear_gradient(
      {{bounds_.origin.x, bounds_.origin.y - scroll_offset + y_screen_offset},
       bounds_.width,
       bounds_.height},
      color1_, color2_, direction_);
}

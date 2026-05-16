#include "browser/TabRenderer.h"
#include "config/Config.h"
#include "gfx/GraphicsContext.h"
#include "layout/LayoutTree.h"
#include <algorithm>

TabRenderer::TabRenderer() = default;

void TabRenderer::render(gfx::GraphicsContext &ctx, int y_screen_offset,
                         int window_height, int document_height,
                         int window_width) const {
  for (const auto &command : display_list_) {
    command->execute(current_scroll_, y_screen_offset, ctx);
  }
  render_scrollbar(ctx, y_screen_offset, document_height, window_height,
                   window_width);
}

void TabRenderer::render_scrollbar(gfx::GraphicsContext &ctx,
                                   int y_screen_offset, int document_height,
                                   int window_height, int window_width) const {
  int viewport_height = window_height - y_screen_offset;

  if (document_height <= viewport_height)
    return; // Story: No need to scroll if content fits in viewport

  int bar_width = config::SCROLLBAR_WIDTH;
  int bar_x = window_width - bar_width;

  // Story: Draw the Scrollbar Track
  ctx.draw_rect({{bar_x, y_screen_offset}, bar_width, viewport_height},
                gfx::Color::from_rgb(240, 240, 240));

  // Story: Draw the Scrollbar Thumb
  double thumb_ratio = (double)viewport_height / document_height;
  int thumb_height = (int)(viewport_height * thumb_ratio);
  if (thumb_height < 20)
    thumb_height = 20; // Minimum size

  double scroll_ratio =
      (double)current_scroll_ / (document_height - viewport_height);
  int thumb_y =
      y_screen_offset + (int)(scroll_ratio * (viewport_height - thumb_height));

  ctx.draw_rect({{bar_x + 2, thumb_y}, bar_width - 4, thumb_height},
                gfx::Color::from_rgb(160, 160, 160));
}

void TabRenderer::scroll_down(int document_height, int window_height) {
  int max_scroll_offset =
      std::max(0, document_height - (window_height - config::UI_HEIGHT));
  current_scroll_ =
      std::min(current_scroll_ + config::SCROLL_STEP, max_scroll_offset);
}

void TabRenderer::scroll_up() {
  current_scroll_ = std::max(0, current_scroll_ - config::SCROLL_STEP);
}

void TabRenderer::rebuild_display_list(const DocumentLayout &document_layout) {
  display_list_.clear();
  paint_tree(document_layout, display_list_);
}

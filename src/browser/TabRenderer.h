#pragma once

#include "layout/DisplayItem.h"
#include "layout/DocumentLayout.h"
#include <memory>
#include <vector>

namespace gfx {
class GraphicsContext;
}

/**
 * Story: Manages rendering state and display list for a Tab.
 *
 * Use-case: Handles painting the document layout, managing the scrollbar,
 * and maintaining the display list of draw commands.
 */
class TabRenderer {
public:
  TabRenderer();

  /**
   * Story: Paints the tab's content into the graphics context.
   * Why: Separates rendering concerns from Tab orchestration.
   * How: Iterates over the display list and executes draw commands.
   */
  void render(gfx::GraphicsContext &ctx, int y_screen_offset, int window_height,
              int document_height, int window_width) const;

  /**
   * Story: Renders the scrollbar on the right side of the screen.
   * Why: To provide visual feedback on scroll position.
   * How: Calculates handle size and position based on content height.
   */
  void render_scrollbar(gfx::GraphicsContext &ctx, int y_screen_offset,
                        int document_height, int window_height,
                        int window_width) const;

  void scroll_down(int document_height, int window_height);
  void scroll_up();

  int current_scroll() const { return current_scroll_; }
  bool is_dragging_scrollbar() const { return is_dragging_scrollbar_; }
  void set_dragging_scrollbar(bool dragging) {
    is_dragging_scrollbar_ = dragging;
  }
  void set_scroll(int value) { current_scroll_ = value; }

  /**
   * Story: Rebuilds the display list from the layout tree.
   * Why: To refresh the visual representation after layout changes.
   * How: Calls paint_tree on the document layout.
   */
  void rebuild_display_list(const DocumentLayout &document_layout);

private:
  int current_scroll_ = 0;
  bool is_dragging_scrollbar_ = false;
  std::vector<std::unique_ptr<DrawCommand>> display_list_;
};

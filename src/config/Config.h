#pragma once
#include <string_view>

/**
 * Global Configuration for C Surfer.
 *
 * This module centralizes all numeric constants used across the browser
 * to ensure consistency, eliminate magic numbers, and prepare for
 * thread-safe configuration access.
 */
namespace config {

  /**
   * Story: Paths for local persistent storage.
   */
  inline constexpr std::string_view COOKIE_FILE_NAME = ".csurfer_cookies";

/**
 * Story: These define the initial "canvas" for the browser. They are used
 * to initialize the SDL window and the Skia rendering surface.
 */
inline constexpr int WINDOW_WIDTH = 800;
inline constexpr int WINDOW_HEIGHT = 600;

/**
 * Story: The 'Chrome' of the browser. UI_HEIGHT is the vertical space
 * reserved for tabs and the address bar. TAB_WIDTH determines the
 * horizontal footprint of a single tab button.
 */
inline constexpr int UI_HEIGHT = 80;
inline constexpr int UI_PADDING = 10;
inline constexpr int TAB_HEIGHT = 35;
inline constexpr int ADDR_HEIGHT = 35;
inline constexpr int TAB_WIDTH = 150;
inline constexpr int SCROLL_STEP = 100;

/**
 * Story: Reserves a narrow gutter on the right side of the tab for
 * vertical scrolling interaction.
 */
inline constexpr int SCROLLBAR_WIDTH = 12;
inline constexpr int SCROLLBAR_MIN_THUMB_HEIGHT = 20;

/**
 * Story: Standard horizontal and vertical padding (H_STEP, V_STEP)
 * added to the document root to prevent content from touching the edges.
 */
inline constexpr int H_STEP = 20;
inline constexpr int V_STEP = 20;
inline constexpr int DOCUMENT_BOTTOM_PADDING = 60;
inline constexpr int SCROLLBAR_DOCUMENT_PADDING = 100;

/**
 * Story: Fallback dimensions for text and form elements when CSS
 * does not provide specific sizing instructions.
 */
inline constexpr int DEFAULT_FONT_SIZE = 16;
inline constexpr int DEFAULT_INPUT_WIDTH = 200;

/**
 * Story: UI layout specific dimensions.
 */
inline constexpr int ADDRESS_BAR_WIDTH = 600;
inline constexpr int TAB_CLOSE_BUTTON_WIDTH = 25;
inline constexpr int UI_TAB_GAP = 5;
inline constexpr int UI_TAB_TEXT_PADDING = 5;
inline constexpr int UI_TAB_CLOSE_OFFSET = 20;
inline constexpr int UI_ICON_OFFSET_X = 13;
inline constexpr int ADDR_BAR_PADDING = 5;

/**
 * Story: The size of the memory buffer used when reading raw data
 * from TCP sockets during HTTP requests.
 */
inline constexpr int HTTP_BUFFER_SIZE = 4096;

/**
 * Story: Target frame delay for ~60 FPS.
 */
inline constexpr int FRAME_DELAY_MS = 16;

} // namespace config

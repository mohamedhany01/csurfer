#pragma once
#include <cstdint>
#include <string>
#include <string_view>

namespace gfx {

/**
 * Story: Represents an RGBA color with 8-bit precision per channel.
 *
 * Use-case: Used throughout the engine for styling, painting, and
 * backend-specific rendering (Skia/SDL). It encapsulates the complexity
 * of color parsing from names and hex codes.
 */
struct Color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t alpha;

  /**
   * Use-case: Create a color from individual integer components.
   * Assumes full opacity if alpha is not provided.
   */
  static Color from_rgb(uint8_t red, uint8_t green, uint8_t blue);
  static Color from_rgba(uint8_t red, uint8_t green, uint8_t blue,
                         uint8_t alpha);

  /**
   * Use-case: Parse a CSS color name (e.g., "red", "cornflowerblue").
   */
  static Color from_name(std::string_view name);

  /**
   * Use-case: Parse a hex string (e.g., "#FF0000" or "#F00").
   */
  static Color from_hex(std::string_view hex);

  // Common Constants
  static const Color Black();
  static const Color White();
  static const Color Transparent();
};

} // namespace gfx

#pragma once
#include <cstdint>

namespace gfx {

struct Color {
  uint8_t r, g, b, a;

  static Color FromRGB(uint8_t r, uint8_t g, uint8_t b) {
    return {r, g, b, 255};
  }

  static Color FromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return {r, g, b, a};
  }

  static const Color Black() { return {0, 0, 0, 255}; }
  static const Color White() { return {255, 255, 255, 255}; }
  static const Color Transparent() { return {0, 0, 0, 0}; }
};

} // namespace gfx

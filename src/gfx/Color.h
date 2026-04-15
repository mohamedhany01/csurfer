#pragma once
#include <cstdint>
#include <string>

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

  static Color FromName(const char *name) {
    if (std::string(name) == "white")
      return White();
    if (std::string(name) == "black")
      return Black();
    if (std::string(name) == "red")
      return {255, 0, 0, 255};
    if (std::string(name) == "green")
      return {0, 128, 0, 255};
    if (std::string(name) == "blue")
      return {0, 0, 255, 255};
    if (std::string(name) == "yellow")
      return {255, 255, 0, 255};
    if (std::string(name) == "gray")
      return {128, 128, 128, 255};
    return Black();
  }
};

} // namespace gfx

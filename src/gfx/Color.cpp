#include "Color.h"
#include <charconv>
#include <unordered_map>

namespace gfx {

Color Color::from_rgb(uint8_t red, uint8_t green, uint8_t blue) {
  return {red, green, blue, 255};
}

Color Color::from_rgba(uint8_t red, uint8_t green, uint8_t blue,
                       uint8_t alpha) {
  return {red, green, blue, alpha};
}

const Color Color::Black() { return {0, 0, 0, 255}; }
const Color Color::White() { return {255, 255, 255, 255}; }
const Color Color::Transparent() { return {0, 0, 0, 0}; }

/**
 * Story: Map of standard web colors.
 * Use-case: Scalable storage for all CSS-named colors.
 */
static const std::unordered_map<std::string_view, Color> COLOR_MAP = {
    {"white", {255, 255, 255, 255}},  {"black", {0, 0, 0, 255}},
    {"red", {255, 0, 0, 255}},        {"green", {0, 128, 0, 255}},
    {"blue", {0, 0, 255, 255}},       {"yellow", {255, 255, 0, 255}},
    {"gray", {128, 128, 128, 255}},   {"orange", {255, 165, 0, 255}},
    {"purple", {128, 0, 128, 255}},   {"cyan", {0, 255, 255, 255}},
    {"magenta", {255, 0, 255, 255}},  {"pink", {255, 192, 203, 255}},
    {"brown", {165, 42, 42, 255}},    {"gold", {255, 215, 0, 255}},
    {"silver", {192, 192, 192, 255}}, {"none", {0, 0, 0, 0}}};

Color Color::from_name(std::string_view name) {
  if (COLOR_MAP.contains(name)) {
    return COLOR_MAP.at(name);
  }

  // Fallback for hex codes if incorrectly passed to from_name
  if (!name.empty() && name[0] == '#') {
    return from_hex(name);
  }

  return Black();
}

/**
 * Story: Efficiently parses hex color strings.
 * Use-case: Supports #RGB (3-digit) and #RRGGBB (6-digit) formats.
 */
Color Color::from_hex(std::string_view hex) {
  if (hex.empty() || hex[0] != '#') {
    return Black();
  }

  std::string_view raw = hex.substr(1);
  uint32_t val = 0;
  auto [ptr, ec] =
      std::from_chars(raw.data(), raw.data() + raw.size(), val, 16);

  if (ec != std::errc()) {
    return Black();
  }

  if (raw.size() == 3) {
    uint8_t red = ((val >> 8) & 0xF) * 17;
    uint8_t green = ((val >> 4) & 0xF) * 17;
    uint8_t blue = (val & 0xF) * 17;
    return {red, green, blue, 255};
  }

  if (raw.size() == 6) {
    uint8_t red = (val >> 16) & 0xFF;
    uint8_t green = (val >> 8) & 0xFF;
    uint8_t blue = val & 0xFF;
    return {red, green, blue, 255};
  }

  return Black();
}

} // namespace gfx

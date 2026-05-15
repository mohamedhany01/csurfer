#pragma once
#include "gfx/Color.h"
#include <sstream>
#include <string>
#include <vector>

namespace CSS {

/**
 * Story: A utility for parsing CSS linear-gradient values.
 *
 * Use-case: Extracts direction and colors from strings like
 * "linear-gradient(to right, red, blue)".
 */
inline bool parse_linear_gradient(const std::string &value,
                                  std::string &direction, gfx::Color &color1,
                                  gfx::Color &color2) {
  if (value.find("linear-gradient(") != 0)
    return false;

  size_t start_index = 16; // length of "linear-gradient("
  size_t end_index = value.find_last_of(')');
  if (end_index == std::string::npos || end_index <= start_index)
    return false;

  std::string content = value.substr(start_index, end_index - start_index);
  std::stringstream ss(content);
  std::string part;
  std::vector<std::string> parts;

  while (std::getline(ss, part, ',')) {
    // Trim whitespace
    size_t first = part.find_first_not_of(" \t");
    size_t last = part.find_last_not_of(" \t");
    if (first != std::string::npos) {
      parts.push_back(part.substr(first, last - first + 1));
    }
  }

  if (parts.size() < 2)
    return false;

  if (parts.size() == 2) {
    direction = "to bottom";
    color1 = gfx::Color::from_name(parts[0]);
    color2 = gfx::Color::from_name(parts[1]);
    return true;
  }
  if (parts.size() == 3) {
    direction = parts[0];
    color1 = gfx::Color::from_name(parts[1]);
    color2 = gfx::Color::from_name(parts[2]);
  }

  return true;
}

} // namespace CSS

#include "utils/StringUtils.h"
#include <algorithm>
#include <cctype>

namespace utils {

std::string to_lower(std::string_view str) {
  std::string result(str);
  std::transform(
      result.begin(), result.end(), result.begin(),
      [](unsigned char character) { return std::tolower(character); });
  return result;
}

std::string trim(std::string_view str) {
  if (str.empty())
    return "";

  size_t first_non_whitespace_index = str.find_first_not_of(" \t\n\r");
  if (first_non_whitespace_index == std::string_view::npos)
    return "";

  size_t last_non_whitespace_index = str.find_last_not_of(" \t\n\r");
  return std::string(
      str.substr(first_non_whitespace_index,
                 (last_non_whitespace_index - first_non_whitespace_index + 1)));
}

} // namespace utils

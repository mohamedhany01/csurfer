#include "utils/StringUtils.h"
#include <algorithm>
#include <cctype>

namespace utils {

std::string to_lower(std::string_view text) {
  std::string result(text);
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return result;
}

std::string trim(std::string_view text) {
  if (text.empty())
    return "";

  size_t first = text.find_first_not_of(" \t\n\r");
  if (first == std::string_view::npos)
    return "";

  size_t last = text.find_last_not_of(" \t\n\r");
  return std::string(text.substr(first, (last - first + 1)));
}

} // namespace utils

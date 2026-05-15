#include "utils/StringUtils.h"
#include <algorithm>
#include <cctype>

namespace utils {

std::string to_lower(std::string_view str) {
  std::string result(str);
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return result;
}

std::string trim(std::string_view str) {
  if (str.empty())
    return "";

  size_t first = str.find_first_not_of(" \t\n\r");
  if (first == std::string_view::npos)
    return "";

  size_t last = str.find_last_not_of(" \t\n\r");
  return std::string(str.substr(first, (last - first + 1)));
}

} // namespace utils

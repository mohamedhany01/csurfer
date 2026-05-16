#pragma once
#include <string>
#include <string_view>

/**
 * Modern C++ String Utilities.
 *
 * Provides pure functions for string
 * manipulation. Using std::string_view for performance to avoid unnecessary
 * copies.
 */
namespace utils {

/**
 * Story: Converts all characters in a string to lowercase.
 * Why: Essential for case-insensitive comparisons (e.g., HTML tags, CSS
 * properties). How: Iterates through the string and applies std::tolower to
 * each character.
 */
std::string to_lower(std::string_view text);

/**
 * Story: Removes leading and trailing whitespace.
 * Why: Crucial for parsing HTTP headers and CSS values where extra spaces are
 * common. How: Finds the first and last non-whitespace characters and returns
 * the substring between them.
 */
std::string trim(std::string_view text);

} // namespace utils

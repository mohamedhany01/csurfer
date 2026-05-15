#pragma once
#include <string>
#include <string_view>

/**
 * Modern C++ String Utilities.
 *
 * SOLID: Single Responsibility - Provides pure functions for string
 * manipulation. Using std::string_view for performance to avoid unnecessary
 * copies.
 */
namespace utils {

/**
 * Story: Converts all characters in a string to lowercase.
 * Essential for case-insensitive comparisons (e.g., HTML tags, CSS properties).
 */
std::string to_lower(std::string_view str);

/**
 * Story: Removes leading and trailing whitespace.
 * Crucial for parsing HTTP headers and CSS values where extra spaces are
 * common.
 */
std::string trim(std::string_view str);

} // namespace utils

#pragma once
#include <string>
#include <string_view>

/**
 * Story: Modern C++ String Utilities.
 *
 * Use-case: Provides pure functions for string manipulation using
 * std::string_view for performance to avoid unnecessary copies.
 */
namespace utils {

/**
 * Story: Converts all characters in a string to lowercase.
 * This is necessary for case-insensitive comparisons in HTML and CSS.
 *
 * Use-case: Normalizing "DIV" or "Div" to "div" before searching for
 * it in our layout rules.
 */
std::string to_lower(std::string_view text);

/**
 * Story: Removes leading and trailing whitespace characters.
 *
 * Use-case: Stripping "  text/html  " to "text/html" during HTTP response
 * parsing.
 */
std::string trim(std::string_view text);

} // namespace utils

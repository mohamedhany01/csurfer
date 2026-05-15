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
 * This is necessary for case-insensitive comparisons in HTML and CSS,
 * as the web standards often treat tag names and properties as
 * case-insensitive, while C++ string comparisons are strictly case-sensitive.
 *
 * Use-case: Normalizing "DIV" or "Div" to "div" before searching for
 * it in our layout rules.
 */
std::string to_lower(std::string_view str);

/**
 * Story: Removes leading and trailing whitespace characters (spaces, tabs,
 * newlines, carriage returns). This is vital for parsing HTTP headers and CSS
 * values where extra padding is common and would otherwise cause string
 * comparison failures.
 *
 * Use-case: Stripping "  text/html  " to "text/html" during HTTP response
 * parsing.
 */
std::string trim(std::string_view str);

} // namespace utils

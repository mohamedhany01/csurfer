#pragma once
#include <string>
#include <vector>
namespace utils {

/**
 * Story: UTF-8 is a variable-length encoding where a single character can
 * occupy 1 to 4 bytes. This function inspects the first byte to determine
 * the total length of the character.
 *
 * Use-case: Essential for correctly iterating over strings that contain
 * international characters or emojis, ensuring we don't break them
 * mid-sequence.
 */
int utf8_character_length(unsigned char first_byte);

/**
 * Story: Breaks a text string into a list of individual words and newline
 * tokens.
 *
 * Use-case: The layout engine uses this to determine where it can safely wrap
 * lines. If a word is too long for the current line, it is moved to the next.
 */
std::vector<std::string> split_into_words(const std::string &text);

/**
 * Story: Checks if a string contains only "boring" whitespace (spaces, tabs).
 *
 * Use-case: Used during DOM tree construction to identify and discard text
 * nodes that only contain formatting whitespace, keeping the tree clean.
 */
bool is_whitespace_only(const std::string &text);

/**
 * Story: Identifies if a character belongs to the Chinese, Japanese, or Korean
 * scripts. These scripts don't use spaces between words.
 *
 * Use-case: Tells the layout engine that it's safe to break a line *anywhere*
 * between these characters, unlike English where we must only break at spaces.
 */
bool is_chinese_japanese_korean(const std::string &character);

/**
 * Story: Converts "unsafe" characters in a string into the %xx format.
 *
 * Use-case: Necessary for encoding form data or query parameters before
 * sending them to a server, ensuring special characters like '&' or '=' don't
 * break the URL structure.
 */
std::string url_percent_encode(const std::string &text);

} // namespace utils

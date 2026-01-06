#pragma once
#include <string>
#include <vector>
namespace utils {
int utf8CharLen(unsigned char c);
std::vector<std::string> splitWords(const std::string &text);
bool isWhitespace(const std::string &s);
bool isCJK(const std::string &ch);
} // namespace utils

#include "Parser.h"

namespace utils {

std::string lex(const std::string &body) {
  std::string text_without_tags;
  bool in_tag = false;

  for (char c : body) {
    if (c == '<')
      in_tag = true;
    else if (c == '>')
      in_tag = false;
    else if (!in_tag)
      text_without_tags += c;
  }

  return text_without_tags;
}

int utf8CharLen(unsigned char c) {
  if ((c & 0x80) == 0x00)
    return 1;
  if ((c & 0xE0) == 0xC0)
    return 2;
  if ((c & 0xF0) == 0xE0)
    return 3;
  if ((c & 0xF8) == 0xF0)
    return 4;
  return 1;
}

} // namespace utils

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

bool isWhitespace(const std::string &s) { return s == " " || s == "\t"; }

bool isCJK(const std::string &ch) {
  unsigned char c = ch[0];
  return (c & 0xF0) == 0xE0; // most CJK chars are 3 bytes
}

std::vector<std::string> splitWords(const std::string &text) {
  std::vector<std::string> words;
  std::string current;

  for (size_t i = 0; i < text.size();) {
    unsigned char c = text[i];

    if (c == '\n') {
      if (!current.empty()) {
        words.push_back(current);
        current.clear();
      }
      words.emplace_back("\n");
      i++;
      continue;
    }

    int len = utf8CharLen(c);
    std::string ch = text.substr(i, len);

    if (isWhitespace(ch)) {
      if (!current.empty()) {
        words.push_back(current);
        current.clear();
      }
      i += len;
      continue;
    }

    if (isCJK(ch)) {
      if (!current.empty()) {
        words.push_back(current);
        current.clear();
      }
      words.push_back(ch);
      i += len;
      continue;
    }

    current += ch;
    i += len;
  }

  if (!current.empty())
    words.push_back(current);

  return words;
}

} // namespace utils

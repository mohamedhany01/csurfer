#include "Parser.h"
#include <format>

namespace utils {

int utf8_character_length(unsigned char first_byte) {
  if ((first_byte & 0x80) == 0x00)
    return 1;
  if ((first_byte & 0xE0) == 0xC0)
    return 2;
  if ((first_byte & 0xF0) == 0xE0)
    return 3;
  if ((first_byte & 0xF8) == 0xF0)
    return 4;
  return 1;
}

bool is_whitespace_only(const std::string &text) {
  return text == " " || text == "\t";
}

bool is_chinese_japanese_korean(const std::string &character_string) {
  unsigned char first_byte = static_cast<unsigned char>(character_string[0]);
  return (first_byte & 0xF0) == 0xE0; // most CJK chars are 3 bytes
}

std::vector<std::string> split_into_words(const std::string &text) {
  std::vector<std::string> words;
  std::string current_word;

  for (size_t index = 0; index < text.size();) {
    unsigned char first_byte = static_cast<unsigned char>(text[index]);

    if (first_byte == '\n') {
      if (!current_word.empty()) {
        words.push_back(current_word);
        current_word.clear();
      }
      words.emplace_back("\n");
      index++;
      continue;
    }

    int length = utf8_character_length(first_byte);
    std::string character_string = text.substr(index, length);

    if (is_whitespace_only(character_string)) {
      if (!current_word.empty()) {
        words.push_back(current_word);
        current_word.clear();
      }
      index += length;
      continue;
    }

    if (is_chinese_japanese_korean(character_string)) {
      if (!current_word.empty()) {
        words.push_back(current_word);
        current_word.clear();
      }
      words.push_back(character_string);
      index += length;
      continue;
    }

    current_word += character_string;
    index += length;
  }

  if (!current_word.empty())
    words.push_back(current_word);

  return words;
}

std::string url_percent_encode(const std::string &text) {
  std::string result;
  for (unsigned char character : text) {
    if (isalnum(character) || character == '-' || character == '_' ||
        character == '.' || character == '~') {
      result += static_cast<char>(character);
    } else {
      result += std::format("%{:02X}", static_cast<unsigned int>(character));
    }
  }
  return result;
}

} // namespace utils

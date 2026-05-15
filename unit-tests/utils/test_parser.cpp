#include "utils/Parser.h"
#include <gtest/gtest.h>

/**
 * Story: We test our linguistic and structural parsers to ensure that
 * they handle complex cases like UTF-8 encodings and East Asian languages
 * correctly. This is vital for a browser that aims to support the global web.
 */

TEST(ParserTest, Utf8CharacterLength) {
  // ASCII (1 byte)
  EXPECT_EQ(utils::utf8_character_length('A'), 1);
  // 2-byte char (e.g. £)
  EXPECT_EQ(utils::utf8_character_length(0xC2), 2);
  // 3-byte char (e.g. €)
  EXPECT_EQ(utils::utf8_character_length(0xE2), 3);
  // 4-byte char (e.g. 🌍)
  EXPECT_EQ(utils::utf8_character_length(0xF0), 4);
}

TEST(ParserTest, SplitIntoWords) {
  std::string text = "Hello world\nNew line";
  auto words = utils::split_into_words(text);

  std::vector<std::string> expected = {"Hello", "world", "\n", "New", "line"};
  EXPECT_EQ(words, expected);
}

TEST(ParserTest, ChineseJapaneseKoreanDetection) {
  // '你好' (Ni Hao) starts with 0xE4 0xBD 0xA0
  EXPECT_TRUE(utils::is_chinese_japanese_korean("\xE4\xBD\xA0"));
  EXPECT_FALSE(utils::is_chinese_japanese_korean("A"));
}

TEST(ParserTest, UrlPercentEncoding) {
  EXPECT_EQ(utils::url_percent_encode("hello world"), "hello%20world");
  EXPECT_EQ(utils::url_percent_encode("A&B"), "A%26B");
  EXPECT_EQ(utils::url_percent_encode("path/to/file"), "path%2Fto%2Ffile");
}

TEST(ParserTest, WhitespaceDetection) {
  EXPECT_TRUE(utils::is_whitespace_only(" "));
  EXPECT_TRUE(utils::is_whitespace_only("\t"));
  EXPECT_FALSE(utils::is_whitespace_only("A"));
}

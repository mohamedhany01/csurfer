#include "utils/StringUtils.h"
#include <gtest/gtest.h>

/**
 * Story: We test our string utilities with various edge cases (empty strings,
 * all-whitespace strings, special characters) to ensure that our parsers
 * receive clean, predictable data.
 */

TEST(StringUtilsTest, ToLower) {
  EXPECT_EQ(utils::to_lower("HeLLo"), "hello");
  EXPECT_EQ(utils::to_lower("WORLD"), "world");
  EXPECT_EQ(utils::to_lower("123!@#"), "123!@#");
  EXPECT_EQ(utils::to_lower(""), "");
}

TEST(StringUtilsTest, Trim) {
  EXPECT_EQ(utils::trim("  hello  "), "hello");
  EXPECT_EQ(utils::trim("hello"), "hello");
  EXPECT_EQ(utils::trim("   "), "");
  EXPECT_EQ(utils::trim("\t\n hello \r"), "hello");
  EXPECT_EQ(utils::trim(""), "");
  EXPECT_EQ(utils::trim("a"), "a");
}

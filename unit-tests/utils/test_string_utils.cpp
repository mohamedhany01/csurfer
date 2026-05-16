#include "utils/StringUtils.h"
#include <gtest/gtest.h>

/**
 * Story: We test our string utilities with various edge cases (empty strings,
 * all-whitespace strings, special characters) to ensure that our parsers
 * receive clean, predictable data.
 */

TEST(StringUtilsTest, ToLower) {
  // Arrange
  struct TestCase {
    std::string input;
    std::string expected;
  };
  std::vector<TestCase> test_cases = {
      {"HeLLo", "hello"}, {"WORLD", "world"}, {"123!@#", "123!@#"}, {"", ""}};

  for (const auto &test_case : test_cases) {
    // Act
    std::string result = utils::to_lower(test_case.input);

    // Assert
    EXPECT_EQ(result, test_case.expected);
  }
}

TEST(StringUtilsTest, Trim) {
  // Arrange
  struct TestCase {
    std::string input;
    std::string expected;
  };
  std::vector<TestCase> test_cases = {
      {"  hello  ", "hello"},     {"hello", "hello"}, {"   ", ""},
      {"\t\n hello \r", "hello"}, {"", ""},           {"a", "a"}};

  for (const auto &test_case : test_cases) {
    // Act
    std::string result = utils::trim(test_case.input);

    // Assert
    EXPECT_EQ(result, test_case.expected);
  }
}

#pragma once

#include "CSSRule.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * Story: A recursive-descent parser for CSS.
 *
 * Use-case: Converts CSS source text into a list of CSSRule objects.
 * It handles selectors, property blocks, and basic error recovery.
 */
class CSSParser {
public:
  explicit CSSParser(std::string css_text);

  /**
   * Story: Parses the full text and returns all valid CSS rules.
   */
  std::vector<CSSRule> parse();

  /**
   * Story: Static helper for parsing a standalone selector (used by JS).
   */
  static std::shared_ptr<CSSSelector> parse_selector(std::string selector_text);

private:
  /**
   * Story: Advances the position past any whitespace.
   */
  void consume_whitespace();

  /**
   * Story: Expects and consumes a specific literal character.
   */
  void consume_literal(char expected_character);

  /**
   * Story: Consumes a word (alpha-numeric plus CSS-valid symbols).
   */
  std::string consume_word();

  /**
   * Story: Consumes a property value until a terminator (; or }) is found.
   */
  std::string consume_value();

  /**
   * Story: Parses a single property:value declaration.
   */
  std::pair<std::string, std::string> consume_declaration();

  /**
   * Story: Parses the contents of a rule block { ... }.
   */
  std::unordered_map<std::string, std::string> consume_body();

  /**
   * Story: Parses a selector, including descendants.
   */
  std::shared_ptr<CSSSelector> consume_selector();

  /**
   * Story: Error recovery helper. Skips input until one of the stop characters
   * is found.
   */
  char ignore_until(const std::vector<char> &stop_characters);

  std::string text_;
  size_t position_ = 0;
};

#pragma once

#include "CSSRule.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class CSSParser {
public:
  explicit CSSParser(std::string css_text);

  // Parses the entire CSS string provided in the constructor.
  std::vector<CSSRule> parse();

private:
  void whitespace();
  void literal(char c);
  std::string word();
  std::pair<std::string, std::string> pair();
  std::unordered_map<std::string, std::string> body();
  std::shared_ptr<CSSSelector> selector();

  // Helper to recover from parsing errors by ignoring characters up to certain
  // stops.
  char ignore_until(const std::vector<char> &chars);

  std::string text_;
  size_t pos_;
};

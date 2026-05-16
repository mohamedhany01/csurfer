#include "CSSParser.h"
#include "utils/StringUtils.h"
#include <algorithm>
#include <cctype>
#include <stdexcept>

CSSParser::CSSParser(std::string css_text)
    : text_(std::move(css_text)), pos_(0) {}

void CSSParser::whitespace() {
  while (pos_ < text_.length() &&
         std::isspace(static_cast<unsigned char>(text_[pos_]))) {
    pos_++;
  }
}

void CSSParser::literal(char c) {
  if (pos_ < text_.length() && text_[pos_] == c) {
    pos_++;
  } else {
    throw std::runtime_error("Parsing error: expected literal");
  }
}

std::string CSSParser::word() {
  size_t start = pos_;
  while (pos_ < text_.length()) {
    char c = text_[pos_];
    if (std::isalnum(static_cast<unsigned char>(c)) || c == '#' || c == '-' ||
        c == '.' || c == '%') {
      pos_++;
    } else {
      break;
    }
  }
  return text_.substr(start, pos_ - start);
}

std::string CSSParser::value() {
  size_t start = pos_;
  while (pos_ < text_.length() && text_[pos_] != ';' && text_[pos_] != '}') {
    pos_++;
  }
  return utils::trim(text_.substr(start, pos_ - start));
}

std::pair<std::string, std::string> CSSParser::pair() {
  std::string prop = word();
  whitespace();
  literal(':');
  whitespace();
  std::string val = value();
  return {utils::to_lower(prop), val};
}

char CSSParser::ignore_until(const std::vector<char> &chars) {
  while (pos_ < text_.length()) {
    char c = text_[pos_];
    if (std::find(chars.begin(), chars.end(), c) != chars.end()) {
      return c;
    }
    pos_++;
  }
  return '\0'; // Return null char if end of string is reached
}

std::unordered_map<std::string, std::string> CSSParser::body() {
  std::unordered_map<std::string, std::string> pairs;
  while (pos_ < text_.length() && text_[pos_] != '}') {
    try {
      auto p = pair();
      pairs[p.first] = p.second;
      whitespace();
      literal(';');
      whitespace();
    } catch (const std::exception &e) {
      char why = ignore_until({';', '}'});
      if (why == ';') {
        literal(';');
        whitespace();
      } else {
        break;
      }
    }
  }
  return pairs;
}

std::shared_ptr<CSSSelector> CSSParser::selector() {
  std::string tag = utils::to_lower(word());
  std::shared_ptr<CSSSelector> out = std::make_shared<TagSelector>(tag);
  whitespace();

  while (pos_ < text_.length() && text_[pos_] != '{') {
    std::string descendant_tag = utils::to_lower(word());
    std::shared_ptr<CSSSelector> descendant =
        std::make_shared<TagSelector>(descendant_tag);
    out = std::make_shared<DescendantSelector>(out, descendant);
    whitespace();
  }
  return out;
}

std::vector<CSSRule> CSSParser::parse() {
  std::vector<CSSRule> rules;
  while (pos_ < text_.length()) {
    try {
      whitespace();
      if (pos_ >= text_.length())
        break;

      auto sel = selector();
      literal('{');
      whitespace();
      auto b = body();
      literal('}');

      rules.push_back({sel, b});
    } catch (const std::exception &e) {
      char why = ignore_until({'}', '{'});
      if (why == '}') {
        literal('}');
        whitespace();
      } else {
        break;
      }
    }
  }
  return rules;
}

std::shared_ptr<CSSSelector>
CSSParser::parse_selector(std::string selector_text) {
  CSSParser parser(std::move(selector_text));
  parser.whitespace();
  return parser.selector();
}

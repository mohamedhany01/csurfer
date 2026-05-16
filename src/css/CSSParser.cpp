#include "CSSParser.h"
#include "utils/Logger.h"
#include "utils/StringUtils.h"
#include <algorithm>
#include <cctype>
#include <stdexcept>

CSSParser::CSSParser(std::string css_text)
    : text_(std::move(css_text)), position_(0) {}

void CSSParser::consume_whitespace() {
  while (position_ < text_.length() &&
         std::isspace(static_cast<unsigned char>(text_[position_]))) {
    position_++;
  }
}

void CSSParser::consume_literal(char expected_character) {
  if (position_ < text_.length() && text_[position_] == expected_character) {
    position_++;
  } else {
    throw std::runtime_error("Parsing error: expected literal '" +
                             std::string(1, expected_character) + "'");
  }
}

std::string CSSParser::consume_word() {
  size_t start_index = position_;
  while (position_ < text_.length()) {
    char current_character = text_[position_];
    if (std::isalnum(static_cast<unsigned char>(current_character)) ||
        current_character == '#' || current_character == '-' ||
        current_character == '.' || current_character == '%' ||
        current_character == '(' || current_character == ')' ||
        current_character == ',') {
      position_++;
    } else {
      break;
    }
  }
  return text_.substr(start_index, position_ - start_index);
}

std::string CSSParser::consume_value() {
  size_t start_index = position_;
  while (position_ < text_.length() && text_[position_] != ';' &&
         text_[position_] != '}') {
    position_++;
  }
  return utils::trim(text_.substr(start_index, position_ - start_index));
}

std::pair<std::string, std::string> CSSParser::consume_declaration() {
  std::string property_name = consume_word();
  consume_whitespace();
  consume_literal(':');
  consume_whitespace();
  std::string property_value = consume_value();
  return {utils::to_lower(property_name), property_value};
}

char CSSParser::ignore_until(const std::vector<char> &stop_characters) {
  while (position_ < text_.length()) {
    char current_character = text_[position_];
    if (std::find(stop_characters.begin(), stop_characters.end(),
                  current_character) != stop_characters.end()) {
      return current_character;
    }
    position_++;
  }
  return '\0';
}

/**
 * Story: The core loop for parsing a single CSS rule block ({ ... }).
 * If an error occurs during property parsing, we attempt to recover by
 * skipping to the next semicolon or the closing brace.
 */
std::unordered_map<std::string, std::string> CSSParser::consume_body() {
  std::unordered_map<std::string, std::string> declarations;
  while (position_ < text_.length() && text_[position_] != '}') {
    try {
      auto declaration_pair = consume_declaration();
      declarations[declaration_pair.first] = declaration_pair.second;
      consume_literal(';');
      consume_whitespace();
    } catch (const std::exception &e) {
      CS_LOG_WARN("[CSS Parser] Recovery: skipping malformed declaration: {}",
                  e.what());
      char stop_char = ignore_until({';', '}'});
      if (stop_char == ';') {
        consume_literal(';');
        consume_whitespace();
      } else {
        break;
      }
    }
  }
  return declarations;
}

std::shared_ptr<CSSSelector> CSSParser::consume_selector() {
  std::string tag_name = utils::to_lower(consume_word());
  std::shared_ptr<CSSSelector> selector =
      std::make_shared<TagSelector>(tag_name);
  consume_whitespace();

  while (position_ < text_.length() && text_[position_] != '{') {
    std::string descendant_tag = utils::to_lower(consume_word());
    if (descendant_tag.empty())
      break;
    std::shared_ptr<CSSSelector> descendant =
        std::make_shared<TagSelector>(descendant_tag);
    selector = std::make_shared<DescendantSelector>(selector, descendant);
    consume_whitespace();
  }
  return selector;
}

/**
 * Story: Parses an entire CSS stylesheet into a list of rules.
 * Errors in a single rule are caught here, allowing the parser to skip to
 * the next block and continue, rather than failing the whole file.
 */
std::vector<CSSRule> CSSParser::parse() {
  std::vector<CSSRule> rules;
  while (position_ < text_.length()) {
    try {
      consume_whitespace();
      if (position_ >= text_.length())
        break;

      auto current_selector = consume_selector();
      consume_literal('{');
      auto declarations = consume_body();
      consume_literal('}');
      rules.push_back({current_selector, declarations});
    } catch (const std::exception &e) {
      CS_LOG_WARN("[CSS Parser] Recovery: skipping malformed rule block: {}",
                  e.what());
      char stop_char = ignore_until({'}', '{'});
      if (stop_char == '}') {
        consume_literal('}');
        consume_whitespace();
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
  parser.consume_whitespace();
  return parser.consume_selector();
}

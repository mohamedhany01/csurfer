#pragma once
#include <string>

enum class LexemeType { Text, Tag };

class Lexeme {
public:
  virtual ~Lexeme() = default;
  virtual LexemeType type() const = 0;

  virtual const std::string &text() const {
    static const std::string empty;
    return empty;
  }

  virtual const std::string &tag() const {
    static const std::string empty;
    return empty;
  }
};

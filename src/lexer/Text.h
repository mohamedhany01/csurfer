#pragma once
#include "Lexeme.h"
#include <string>

class Text : public Lexeme {
public:
  explicit Text(std::string value);

  LexemeType type() const override;
  const std::string &text() const override;

private:
  std::string value_;
};

#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Lexeme.h"

class Lexer {
public:
  explicit Lexer(const std::string &body);

  std::vector<std::unique_ptr<Lexeme>> lex() const;

private:
  const std::string &body_;
};

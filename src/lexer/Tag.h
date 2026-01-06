#pragma once
#include "Lexeme.h"
#include <string>

class Tag : public Lexeme {
public:
  explicit Tag(std::string name);

  LexemeType type() const override;
  const std::string &tag() const override;

private:
  std::string name_;
};

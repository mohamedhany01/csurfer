#pragma once
#include "Element.h"
#include "Lexeme.h"
#include <string>

class Text : public Lexeme {
public:
  explicit Text(std::string text, Element *parent = nullptr);

  LexemeType type() const override;
  const std::string &text() const override;

  Element *parent() const;
  void setParent(Element *parent);

  // Short string form used when debugging the tree,
  // like "'Hello world'".
  std::string get_string() const;

private:
  std::string value_;
  Element *parent_;
};

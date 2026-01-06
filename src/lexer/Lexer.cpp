#include "Lexer.h"
#include "Tag.h"
#include "Text.h"

Lexer::Lexer(const std::string &body) : body_(body) {}

std::vector<std::unique_ptr<Lexeme>> Lexer::lex() const {
  std::vector<std::unique_ptr<Lexeme>> out;

  std::string buffer;
  bool in_tag = false;

  for (char c : body_) {
    if (c == '<') {
      in_tag = true;

      if (!buffer.empty()) {
        out.push_back(std::make_unique<Text>(buffer));
        buffer.clear();
      }

    } else if (c == '>') {
      in_tag = false;

      out.push_back(std::make_unique<Tag>(buffer));
      buffer.clear();

    } else {
      buffer += c;
    }
  }

  if (!in_tag && !buffer.empty()) {
    out.push_back(std::make_unique<Text>(buffer));
  }

  return out;
}

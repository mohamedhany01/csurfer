#pragma once
#include "Lexeme.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Element : public Lexeme {
public:
  using AttributeMap = std::unordered_map<std::string, std::string>;
  using StyleMap = std::unordered_map<std::string, std::string>;

  explicit Element(std::string tag, AttributeMap attributes = {},
                   Element *parent = nullptr);

  LexemeType type() const override;
  const std::string &element() const override;

  const std::string &tag() const;
  const AttributeMap &attributes() const;
  void setAttribute(const std::string &name, const std::string &value);

  Element *parent() const override;
  void setParent(Element *parent);

  const std::vector<std::unique_ptr<Lexeme>> &children() const;
  void appendChild(std::unique_ptr<Lexeme> child);
  void clearChildren();
  void moveChildrenFrom(Element *other);

  // Computed styles for this element
  const StyleMap &style() const;
  void addStyle(const std::string &property, const std::string &value);

  // Short string form used when debugging the tree,
  // like "<p>" or "<a>".
  std::string get_string() const override;

private:
  std::string tag_;
  AttributeMap attributes_;
  StyleMap style_;
  std::vector<std::unique_ptr<Lexeme>> children_;
  Element *parent_;
};

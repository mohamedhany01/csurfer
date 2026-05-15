#pragma once
#include "Element.h"
#include "Lexeme.h"
#include <string>

/**
 * Story: Represents a plain text node in the DOM.
 *
 * Use-case: Used for all text content inside tags (e.g., the "Hello" in
 * <p>Hello</p>). Text nodes are leaf nodes in the tree (they have no children).
 */
class Text : public Lexeme {
public:
  explicit Text(std::string text_content, Element *parent_element = nullptr);

  LexemeType type() const override;

  /**
   * Story: Returns the actual text string.
   */
  const std::string &text() const override;

  /**
   * Story: Returns the parent element.
   */
  Element *parent() const override;

  /**
   * Story: Updates the parent node.
   */
  void set_parent(Element *parent_node);

  /**
   * Story: Debug string (e.g., "'Hello world'").
   */
  std::string get_string() const override;

private:
  std::string value_;
  Element *parent_;
};

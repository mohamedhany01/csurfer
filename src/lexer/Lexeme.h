#pragma once
#include <string>

/**
 * Story: The base class for all nodes in the Document Object Model (DOM).
 *
 * Use-case: This is a "Composite" pattern base. Both Elements (tags) and
 * Text nodes inherit from Lexeme, allowing us to build a tree where
 * mixed types are treated uniformly during traversal and layout.
 */
enum class LexemeType { Text, Element };

class Lexeme {
public:
  virtual ~Lexeme() = default;

  /**
   * Story: Returns the type of node (Text vs Element).
   */
  virtual LexemeType type() const = 0;

  /**
   * Story: Returns the text content of the node.
   * For Elements, this is usually empty or a concatenation of children.
   */
  virtual const std::string &text() const {
    static const std::string empty;
    return empty;
  }

  /**
   * Story: Returns the tag name of the node (e.g., "div", "a").
   * For Text nodes, this is empty.
   */
  virtual const std::string &tag() const {
    static const std::string empty;
    return empty;
  }

  /**
   * Story: Returns a human-readable representation for debugging.
   */
  virtual std::string get_string() const = 0;

  /**
   * Story: Returns the parent element.
   */
  virtual class Element *parent() const = 0;

  /**
   * Story: Returns the focus state of this node.
   */
  bool is_focused() const { return is_focused_; }

  /**
   * Story: Updates the focus state.
   */
  void set_focused(bool is_focused);

protected:
  bool is_focused_ = false;
};

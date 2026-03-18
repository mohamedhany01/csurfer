#pragma once
#include <string>

enum class LexemeType { Text, Element };

class Lexeme {
public:
  virtual ~Lexeme() = default;
  virtual LexemeType type() const = 0;

  virtual const std::string &text() const {
    static const std::string empty;
    return empty;
  }

  /**
   * Get a string representation of the element for logging and debugging.
   */
  virtual std::string get_string() const = 0;

  virtual const std::string &element() const {
    static const std::string empty;
    return empty;
  }

  virtual class Element *parent() const = 0;

  /**
   * Check if the element currently has focus.
   */
  bool is_focused() const { return is_focused_; }

  /**
   * Update the focus state of the element.
   */
  void set_focused(bool f);

protected:
  bool is_focused_ = false;
};

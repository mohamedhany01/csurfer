#include "Lexeme.h"
#include <iostream>

/**
 * Update the focus state of the element and log the change.
 * This helps track element interaction during Chapter 8 development.
 */
void Lexeme::set_focused(bool f) {
  if (is_focused_ != f) {
    is_focused_ = f;
    std::cout << "[Focus] Node " << get_string()
              << (is_focused_ ? " gained " : " lost ") << "focus." << std::endl;
  }
}

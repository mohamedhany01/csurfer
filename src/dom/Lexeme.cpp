#include "Lexeme.h"

/**
 * Story: Updates the internal focus state.
 * Use-case: Used by the Tab manager to track which element receives keyboard
 * input.
 */
void Lexeme::set_focused(bool is_focused) { is_focused_ = is_focused; }

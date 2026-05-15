#pragma once
#include "dom/Element.h"
#include "dom/Lexeme.h"
#include "gfx/Font.h"
#include <memory>
#include <string>

class BlockLayout;

namespace layout {

/**
 * Story: A helper for the complex inline layout process.
 *
 * Use-case: Handles text wrapping, word positioning, and line-breaking
 * for BlockLayout boxes operating in Inline Mode.
 */
class InlineLayoutHelper {
public:
  static void layout(BlockLayout &container);

private:
  static void recurse_node(BlockLayout &container, const Lexeme *node);
  static void start_new_line(BlockLayout &container);
  static void layout_text(BlockLayout &container, const Lexeme *text_node,
                          const std::string &content,
                          const Element *parent_element);
  static void layout_word(BlockLayout &container, const Lexeme *origin_node,
                          const std::string &word_text,
                          const Element *parent_element);
  static void layout_input(BlockLayout &container, const Lexeme *input_node);
  static std::shared_ptr<gfx::Font>
  get_current_font(BlockLayout &container, const Element *element_node);
};

} // namespace layout

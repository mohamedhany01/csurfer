#pragma once
#include "dom/Element.h"
#include "dom/Lexeme.h"
#include "gfx/Font.h"
#include "layout/DisplayItem.h"
#include "layout/LayoutObject.h"
#include <unordered_map>
#include <vector>

/**
 * Story: A cache key for matching font styles (size, weight, style).
 */
struct FontKey {
  int size;
  bool bold;
  bool italic;

  bool operator==(const FontKey &other) const {
    return size == other.size && bold == other.bold && italic == other.italic;
  }
};

struct FontKeyHash {
  std::size_t operator()(const FontKey &key) const {
    return std::hash<int>()(key.size) ^ (std::hash<bool>()(key.bold) << 1) ^
           (std::hash<bool>()(key.italic) << 2);
  }
};

/**
 * Story: The primary layout engine for block and inline elements.
 *
 * Use-case: A BlockLayout box handles two fundamental CSS layout modes:
 * 1. Block Mode: Children are other block boxes stacked vertically.
 * 2. Inline Mode: Children are text or inline elements wrapped into lines.
 */
class BlockLayout final : public LayoutObject {
public:
  BlockLayout(const Lexeme *dom_node, LayoutObject *parent_layout,
              BlockLayout *previous_sibling, gfx::FontManager &font_manager);

  // For anonymous block boxes created to wrap sequences of inline elements
  BlockLayout(std::vector<const Lexeme *> anonymous_children,
              LayoutObject *parent_layout, BlockLayout *previous_sibling,
              gfx::FontManager &font_manager);

  void layout() override;
  void
  paint(std::vector<std::unique_ptr<DrawCommand>> &display_list) const override;

  const Lexeme *node() const override { return node_; }
  float get_opacity() const override;
  std::string get_blend_mode() const override;
  bool is_overflow_clip() const override;
  float get_border_radius() const override;

private:
  const Lexeme *node_;
  LayoutObject *parent_layout_;
  BlockLayout *previous_sibling_;
  gfx::FontManager &font_manager_;

  std::vector<const Lexeme *> anonymous_children_;

  int current_cursor_x_ = 0;

  std::unordered_map<FontKey, std::shared_ptr<gfx::Font>, FontKeyHash>
      font_cache_;

  enum class LayoutMode { Inline, Block };
  LayoutMode determine_layout_mode() const;
  void layout_block_children();
  void layout_inline_children();

  void recurse_node(const Lexeme *current_node);
  void layout_node(const Lexeme *current_node);
  void layout_element(const Element *element_node);
  void layout_text(const Lexeme *text_node, const std::string &content,
                   const Element *parent_element);

  void layout_word(const Lexeme *origin_node, const std::string &word_text,
                   const Element *parent_element);

  void start_new_line();

  void layout_input(const Lexeme *input_node);

  std::shared_ptr<gfx::Font> get_current_font(const Element *element_node);
};

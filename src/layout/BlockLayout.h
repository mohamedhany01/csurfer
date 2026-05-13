#pragma once
#include "gfx/Font.h"
#include "layout/DisplayItem.h"
#include "layout/LayoutObject.h"
#include "lexer/Element.h"
#include "lexer/Lexeme.h"
#include <unordered_map>
#include <vector>

/**
 * Font cache key for matching style states.
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
  std::size_t operator()(const FontKey &k) const {
    return std::hash<int>()(k.size) ^ (std::hash<bool>()(k.bold) << 1) ^
           (std::hash<bool>()(k.italic) << 2);
  }
};

/**
 * A single block in the layout tree.
 *
 * A BlockLayout either:
 *   * stacks child blocks vertically (block mode), or
 *   * lays out inline text into lines (inline mode).
 *
 * Stage 1.2: Decoupled from SDL_ttf by using generic font handles.
 */
class BlockLayout final : public LayoutObject {
public:
  BlockLayout(const Lexeme *node, LayoutObject *parent, BlockLayout *previous,
              gfx::FontManager &font_manager);

  // For anonymous block boxes that hold runs of inline elements
  BlockLayout(std::vector<const Lexeme *> anonymous_children,
              LayoutObject *parent, BlockLayout *previous,
              gfx::FontManager &font_manager);

  void layout() override;
  void paint(std::vector<std::unique_ptr<DrawCommand>> &out) const override;

private:
  const Lexeme *node_;
  LayoutObject *parent_;
  BlockLayout *previous_;
  gfx::FontManager &font_manager_;

  std::vector<const Lexeme *> anonymous_children_;

  int cursor_x_;

  // Font cache using shared_ptr to gfx::Font
  std::unordered_map<FontKey, std::shared_ptr<gfx::Font>, FontKeyHash>
      font_cache_;

  enum class LayoutMode { Inline, Block };
  LayoutMode layout_mode() const;

  void recurse(const Lexeme *node);
  void layoutNode(const Lexeme *node);
  void layoutElement(const Element *element);
  void layoutText(const Lexeme *text_node, const std::string &text,
                  const Element *parent_element);

  void word(const Lexeme *node, const std::string &word,
            const Element *parent_element);

  void new_line();

  void input(const Lexeme *node);

  std::shared_ptr<gfx::Font> currentFont(const Element *element);
};

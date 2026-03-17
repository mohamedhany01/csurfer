#pragma once
#include <unordered_map>
#include <vector>

#include "layout/DisplayItem.h"
#include "layout/LayoutObject.h"
#include "lexer/Element.h"
#include "lexer/Lexeme.h"

struct FontMetrics {
  const int ascent;   // hight from baseline to top
  const int descent;  // hight from baseline to bottom
  const int lineSkip; // line hight
};

// Old LineItem and TextDisplayItem structs were removed here

// Font cache
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

// A single block in the layout tree.
//
// A BlockLayout either:
//   * stacks child blocks vertically (block mode), or
//   * lays out inline text into lines (inline mode).
//
// It computes its own box (x, y, width, height) and then paints text and any
// block background (for example, a gray rectangle for <pre>).
class BlockLayout final : public LayoutObject {
public:
  BlockLayout(const Lexeme *node, LayoutObject *parent, BlockLayout *previous,
              const FontMetrics &metrics);

  void layout() override;
  void paint(std::vector<std::unique_ptr<DrawCommand>> &out) const override;

private:
  // -------- Input --------
  const Lexeme *node_;
  LayoutObject *parent_;
  BlockLayout *previous_;
  const FontMetrics &metrics_;

  // -------- Cursor state --------
  int cursor_x_;
  // cursor_y_ is removed

  // -------- Formatting state --------
  std::unordered_map<FontKey, TTF_Font *, FontKeyHash> font_cache_;

  // -------- Layout buffers --------
  // line_ and display_list_ are removed, LineLayout children manage this now

  // -------- Layout helpers --------
  enum class LayoutMode { Inline, Block };
  LayoutMode layout_mode() const;

  // Build inline content display_list_ for this block.
  void recurse(const Lexeme *node);
  void layoutNode(const Lexeme *node);
  void layoutElement(const Element *element);
  void layoutText(const std::string &text, const Element *parent_element);

  // Add a single word to the current line, styled according to the parent
  // element
  void word(const Lexeme *node, const std::string &word, const Element *parent_element);

  // Start a new line by appending a LineLayout child
  void new_line();

  // Create or select a font matching current element's style state
  TTF_Font *currentFont(const Element *element);
};

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

struct LineItem {
  int rel_x;        // relative to the block
  std::string text; // UTF-8
  TTF_Font *font;   // font used to render this word
};

struct TextDisplayItem {
  int x;            // page coordinate
  int y;            // page coordinate
  std::string text; // UTF-8
  TTF_Font *font;   // font data
};

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
  int cursor_y_;

  // -------- Formatting state --------
  std::unordered_map<FontKey, TTF_Font *, FontKeyHash> font_cache_;
  bool bold_ = false;
  bool italic_ = false;
  int font_size_ = 16;

  // -------- Layout buffers --------
  std::vector<LineItem> line_;                // current line buffer
  std::vector<TextDisplayItem> display_list_; // inline-only output

  // -------- Layout helpers --------
  enum class LayoutMode { Inline, Block };
  LayoutMode layout_mode() const;

  // Build inline content display_list_ for this block.
  void recurse(const Lexeme *node);
  void layoutNode(const Lexeme *node);
  void layoutElement(const Element *element);
  void open_tag(const std::string &tag);
  void close_tag(const std::string &tag);
  void layoutText(const std::string &text);

  // Add a single word to the current line
  void word(const std::string &word);

  // Flush the current line buffer into the display list
  void flush();

  // Create or select a font matching current style state
  TTF_Font *currentFont();
};

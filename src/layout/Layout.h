#pragma once
#include <SDL_ttf.h>
#include <memory>
#include <unordered_map>
#include <vector>

#include "layout/DisplayItem.h"
#include "lexer/Element.h"
#include "lexer/Lexeme.h"

struct FontMetrics {
  const int ascent;   // hight from baseline to top
  const int descent;  // hight from baseline to bottom
  const int lineSkip; // line hight
};

struct LineItem {
  int x;            // horizontal position
  std::string text; // word text
  TTF_Font *font;   // font used to render this word
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

class Layout {
public:
  Layout(const Element &root, const FontMetrics &metrics, int max_width);

  // Produces the final display list ready for rendering
  std::vector<DisplayItem> build();

private:
  // -------- Input --------
  const Element &root_;
  FontMetrics metrics_;
  int max_width_;

  // -------- Cursor state --------
  int cursor_x_;
  int cursor_y_;

  // -------- Formatting state --------
  std::unordered_map<FontKey, TTF_Font *, FontKeyHash> font_cache_;
  bool bold_ = false;
  bool italic_ = false;
  int font_size_ = 16;

  // -------- Layout buffers --------
  std::vector<LineItem> line_;            // current line buffer
  std::vector<DisplayItem> display_list_; // final output

  // -------- Layout helpers --------
  void layoutNode(const Lexeme &node);
  void layoutElement(const Element &element);
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

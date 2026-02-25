#include "layout/Layout.h"
#include "utils/Parser.h"
#include <SDL_ttf.h>

static constexpr int HSTEP = 20;

// Set up layout for a page: remember the root node, font metrics,
// and maximum line width, and place the cursor at the starting point.
Layout::Layout(const Element &root, const FontMetrics &metrics, int max_width)
    : root_(root), metrics_(metrics), max_width_(max_width), cursor_x_(HSTEP),
      cursor_y_(metrics.ascent + 18) {}

// Build the full list of things to draw on screen from the DOM tree.
// This walks the whole tree, lays out text, then flushes the last line.
std::vector<DisplayItem> Layout::build() {
  display_list_.clear();
  line_.clear();

  layoutElement(root_);

  // Flush any remaining buffered words
  flush();

  return display_list_;
}

// Handle a single node in the tree: either text or an element.
// Text becomes words; elements change style and recurse into children.
void Layout::layoutNode(const Lexeme &node) {
  if (node.type() == LexemeType::Text) {
    layoutText(node.text());
    return;
  }

  if (node.type() == LexemeType::Element) {
    const auto *el = dynamic_cast<const Element *>(&node);
    if (!el) {
      return;
    }
    layoutElement(*el);
  }
}

// Handle an element node: apply its tag effects, lay out all children,
// then undo the tag effects after children are done.
void Layout::layoutElement(const Element &element) {
  const std::string &tag = element.tag();

  open_tag(tag);

  for (const auto &child : element.children()) {
    layoutNode(*child);
  }

  close_tag(tag);
}

// Apply style changes when we enter a tag, like turning bold or italic on,
// changing font size, or forcing a line break for <br>.
void Layout::open_tag(const std::string &tag) {
  if (tag == "b") {
    bold_ = true;
  } else if (tag == "i") {
    italic_ = true;
  } else if (tag == "small") {
    font_size_ -= 2;
  } else if (tag == "big") {
    font_size_ += 4;
  } else if (tag == "br") {
    flush();
  }
}

// Undo style changes when we leave a tag, like turning bold or italic off,
// restoring font size, or ending a paragraph for </p>.
void Layout::close_tag(const std::string &tag) {
  if (tag == "b") {
    bold_ = false;
  } else if (tag == "i") {
    italic_ = false;
  } else if (tag == "small") {
    font_size_ += 2;
  } else if (tag == "big") {
    font_size_ -= 4;
  } else if (tag == "p") {
    flush();
    cursor_y_ += metrics_.lineSkip;
  }
}

// Break a block of text into words and newlines, and send each word
// to the word layout logic.
void Layout::layoutText(const std::string &text) {
  auto words = utils::splitWords(text);

  for (const auto &w : words) {
    if (w == "\n") {
      flush();
      continue;
    }
    word(w);
  }
}

// Add one word to the current line, measuring its size and wrapping
// to the next line if it would overflow the available width.
void Layout::word(const std::string &word_text) {
  TTF_Font *font = currentFont();

  int w, h;
  TTF_SizeUTF8(font, word_text.c_str(), &w, &h);

  // Line wrapping
  if (cursor_x_ + w > max_width_) {
    flush();
  }

  // Add word to the current line buffer
  line_.push_back({cursor_x_, word_text, font});

  // Advance cursor (include space width)
  int space_w;
  TTF_SizeUTF8(font, " ", &space_w, nullptr);
  cursor_x_ += w + space_w;
}

// Turn the current line buffer into final draw items at the right
// vertical position, then move the cursor to the next line.
void Layout::flush() {
  if (line_.empty())
    return;

  // Compute max ascent and descent for baseline alignment
  int max_ascent = 0;
  int max_descent = 0;

  for (const auto &item : line_) {
    int ascent = TTF_FontAscent(item.font);
    int descent = std::abs(TTF_FontDescent(item.font));
    max_ascent = std::max(max_ascent, ascent);
    max_descent = std::max(max_descent, descent);
  }

  // Baseline position with leading
  int baseline = cursor_y_ + static_cast<int>(1.25 * max_ascent);

  // Commit words to the display list
  for (const auto &item : line_) {
    int y = baseline - TTF_FontAscent(item.font);
    display_list_.push_back({item.x, y, item.text, item.font});
  }

  // Advance cursor to the next line
  cursor_y_ = baseline + static_cast<int>(1.25 * max_descent);
  cursor_x_ = HSTEP;

  // Clear line buffer
  line_.clear();
}

// Get or create a font that matches the current style (size, bold, italic),
// caching fonts so we do not reload them every time.
TTF_Font *Layout::currentFont() {
  std::string font_path =
      std::string(ASSETS_DIR) + "/fonts/NotoSansCJK-Regular.ttc";

  FontKey key{font_size_, bold_, italic_};

  if (font_cache_.contains(key))
    return font_cache_[key];

  TTF_Font *font = TTF_OpenFont(font_path.c_str(), font_size_);

  int style = TTF_STYLE_NORMAL;
  if (bold_)
    style |= TTF_STYLE_BOLD;
  if (italic_)
    style |= TTF_STYLE_ITALIC;

  TTF_SetFontStyle(font, style);

  font_cache_[key] = font;
  return font;
}

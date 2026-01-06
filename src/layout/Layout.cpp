#include "layout/Layout.h"
#include "utils/Parser.h"
#include <SDL_ttf.h>

static constexpr int HSTEP = 20;

Layout::Layout(const std::vector<std::unique_ptr<Lexeme>> &tokens,
               const FontMetrics &metrics, int max_width)
    : tokens_(tokens), metrics_(metrics), max_width_(max_width),
      cursor_x_(HSTEP), cursor_y_(metrics.ascent + 18) {}

std::vector<DisplayItem> Layout::build() {
  display_list_.clear();
  line_.clear();

  for (const auto &tok : tokens_) {
    handleToken(*tok);
  }

  // Flush any remaining buffered words
  flush();

  return display_list_;
}

void Layout::handleToken(const Lexeme &tok) {
  if (tok.type() == LexemeType::Text) {
    layoutText(tok.text());
  } else if (tok.type() == LexemeType::Tag) {
    const std::string &tag = tok.tag();

    if (tag == "b")
      bold_ = true;
    else if (tag == "/b")
      bold_ = false;
    else if (tag == "i")
      italic_ = true;
    else if (tag == "/i")
      italic_ = false;
    else if (tag == "small")
      font_size_ -= 2;
    else if (tag == "/small")
      font_size_ += 2;
    else if (tag == "big")
      font_size_ += 4;
    else if (tag == "/big")
      font_size_ -= 4;
    else if (tag == "br") {
      // Explicit line break
      flush();
    } else if (tag == "/p") {
      // End paragraph
      flush();
      cursor_y_ += metrics_.lineSkip;
    }
  }
}
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

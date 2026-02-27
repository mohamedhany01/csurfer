#include "layout/BlockLayout.h"

#include "layout/LayoutConstants.h"
#include "utils/Parser.h"

#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <cmath>
#include <unordered_set>

static const std::unordered_set<std::string> BLOCK_ELEMENTS = {
    "html",   "body",       "article", "section",  "nav",        "aside",
    "h1",     "h2",         "h3",      "h4",       "h5",         "h6",
    "hgroup", "header",     "footer",  "address",  "p",          "hr",
    "pre",    "blockquote", "ol",      "ul",       "menu",       "li",
    "dl",     "dt",         "dd",      "figure",   "figcaption", "main",
    "div",    "table",      "form",    "fieldset", "legend",     "details",
    "summary"};

BlockLayout::BlockLayout(const Lexeme *node, LayoutObject *parent,
                         BlockLayout *previous, const FontMetrics &metrics)
    : node_(node), parent_(parent), previous_(previous), metrics_(metrics),
      cursor_x_(0), cursor_y_(0) {}

BlockLayout::LayoutMode BlockLayout::layout_mode() const {
  if (!node_)
    return LayoutMode::Block;

  if (node_->type() == LexemeType::Text) {
    return LayoutMode::Inline;
  }

  const auto *el = dynamic_cast<const Element *>(node_);
  if (!el)
    return LayoutMode::Block;

  bool has_block_child = false;
  for (const auto &child : el->children()) {
    if (child && child->type() == LexemeType::Element) {
      const auto *child_el = dynamic_cast<const Element *>(child.get());
      if (child_el && BLOCK_ELEMENTS.contains(child_el->tag())) {
        has_block_child = true;
        break;
      }
    }
  }

  if (has_block_child)
    return LayoutMode::Block;

  if (!el->children().empty())
    return LayoutMode::Inline;

  return LayoutMode::Block;
}

void BlockLayout::layout() {
  display_list_.clear();
  line_.clear();
  children_.clear();

  x = parent_ ? parent_->x : 0;
  width = parent_ ? parent_->width : 0;

  if (previous_) {
    y = previous_->y + previous_->height;
  } else {
    y = parent_ ? parent_->y : 0;
  }

  const auto mode = layout_mode();
  if (mode == LayoutMode::Block) {
    const auto *el = dynamic_cast<const Element *>(node_);
    if (el) {
      BlockLayout *prev = nullptr;
      for (const auto &child : el->children()) {
        auto next =
            std::make_unique<BlockLayout>(child.get(), this, prev, metrics_);
        prev = next.get();
        children_.push_back(std::move(next));
      }
    }
  } else {
    cursor_x_ = 0;
    cursor_y_ = 0;
    bold_ = false;
    italic_ = false;
    font_size_ = 16;
    recurse(node_);
    flush();
  }

  for (auto &child : children_) {
    child->layout();
  }

  if (mode == LayoutMode::Block) {
    int total = 0;
    for (const auto &child : children_) {
      total += child->height;
    }
    height = total;
  } else {
    height = cursor_y_;
  }
}

void BlockLayout::paint(std::vector<std::unique_ptr<DrawCommand>> &out) const {
  const auto *el = dynamic_cast<const Element *>(node_);
  if (el && el->tag() == "pre") {
    SDL_Color gray{200, 200, 200, 255};
    out.push_back(
        std::make_unique<DrawRect>(x, y, x + width, y + height, gray));
  }

  if (layout_mode() == LayoutMode::Inline) {
    for (const auto &item : display_list_) {
      out.push_back(
          std::make_unique<DrawText>(item.x, item.y, item.text, item.font));
    }
  }
}

void BlockLayout::recurse(const Lexeme *node) { layoutNode(node); }

void BlockLayout::layoutNode(const Lexeme *node) {
  if (!node)
    return;

  if (node->type() == LexemeType::Text) {
    layoutText(node->text());
    return;
  }

  if (node->type() == LexemeType::Element) {
    const auto *el = dynamic_cast<const Element *>(node);
    layoutElement(el);
  }
}

void BlockLayout::layoutElement(const Element *element) {
  if (!element)
    return;

  const std::string &tag = element->tag();
  open_tag(tag);

  for (const auto &child : element->children()) {
    layoutNode(child.get());
  }

  close_tag(tag);
}

void BlockLayout::open_tag(const std::string &tag) {
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

void BlockLayout::close_tag(const std::string &tag) {
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

void BlockLayout::layoutText(const std::string &text) {
  auto words = utils::splitWords(text);
  for (const auto &w : words) {
    if (w == "\n") {
      flush();
      continue;
    }
    word(w);
  }
}

void BlockLayout::word(const std::string &word_text) {
  TTF_Font *font = currentFont();
  if (!font)
    return;

  int w = 0;
  int h = 0;
  TTF_SizeUTF8(font, word_text.c_str(), &w, &h);

  if (cursor_x_ + w > width) {
    flush();
  }

  line_.push_back({cursor_x_, word_text, font});

  int space_w = 0;
  TTF_SizeUTF8(font, " ", &space_w, nullptr);
  cursor_x_ += w + space_w;
}

void BlockLayout::flush() {
  if (line_.empty())
    return;

  int max_ascent = 0;
  int max_descent = 0;
  for (const auto &item : line_) {
    int ascent = TTF_FontAscent(item.font);
    int descent = std::abs(TTF_FontDescent(item.font));
    max_ascent = std::max(max_ascent, ascent);
    max_descent = std::max(max_descent, descent);
  }

  int baseline = cursor_y_ + static_cast<int>(1.25 * max_ascent);
  for (const auto &item : line_) {
    int abs_x = x + item.rel_x;
    int abs_y = y + baseline - TTF_FontAscent(item.font);
    display_list_.push_back({abs_x, abs_y, item.text, item.font});
  }

  cursor_y_ = baseline + static_cast<int>(1.25 * max_descent);
  cursor_x_ = 0;
  line_.clear();
}

TTF_Font *BlockLayout::currentFont() {
  std::string font_path =
      std::string(ASSETS_DIR) + "/fonts/NotoSansCJK-Regular.ttc";

  FontKey key{font_size_, bold_, italic_};
  if (font_cache_.contains(key))
    return font_cache_[key];

  TTF_Font *font = TTF_OpenFont(font_path.c_str(), font_size_);
  if (!font)
    return nullptr;

  int style = TTF_STYLE_NORMAL;
  if (bold_)
    style |= TTF_STYLE_BOLD;
  if (italic_)
    style |= TTF_STYLE_ITALIC;
  TTF_SetFontStyle(font, style);

  font_cache_[key] = font;
  return font;
}

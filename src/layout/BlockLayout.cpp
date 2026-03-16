#include "layout/BlockLayout.h"

#include "lexer/Text.h"
#include "utils/Parser.h"

#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <cmath>
#include <iostream>
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
  if (el) {
    auto styles = el->style();
    if (styles.find("background-color") != styles.end()) {
      std::string bgcolor = styles.at("background-color");
      if (bgcolor != "transparent" && !bgcolor.empty()) {
        // Very simple color parsing for now (assuming named colors or hex)
        // Just map some basic colors to prove it works
        SDL_Color color{200, 200, 200, 255}; // Default Gray
        if (bgcolor == "blue")
          color = {0, 0, 255, 255};
        else if (bgcolor == "red")
          color = {255, 0, 0, 255};
        else if (bgcolor == "green")
          color = {0, 255, 0, 255};
        else if (bgcolor == "yellow")
          color = {255, 255, 0, 255};
        else if (bgcolor == "black")
          color = {0, 0, 0, 255};

        out.push_back(
            std::make_unique<DrawRect>(x, y, x + width, y + height, color));
      }
    }
  }

  if (layout_mode() == LayoutMode::Inline) {
    for (const auto &item : display_list_) {
      out.push_back(std::make_unique<DrawText>(item.x, item.y, item.text,
                                               item.font, item.color));
    }
  }
}

void BlockLayout::recurse(const Lexeme *node) { layoutNode(node); }

void BlockLayout::layoutNode(const Lexeme *node) {
  if (!node)
    return;

  if (node->type() == LexemeType::Text) {
    // A Text node itself doesn't have styles, its parent does.
    const Element *parent_el = nullptr;
    // We need to trace back from LayoutTree or find parent in DOM.
    // Lexeme doesn't have parent() in the interface, but Element/Text do.
    // Fortunately Text has parent(). But node is a Lexeme*.
    if (const auto *t = dynamic_cast<const Text *>(node)) {
      parent_el = dynamic_cast<const Element *>(t->parent());
    }
    layoutText(node->text(), parent_el);
    return;
  }

  if (node->type() == LexemeType::Element) {
    const auto *el = dynamic_cast<const Element *>(node);
    if (el && el->tag() == "br") {
      flush();
    }
    layoutElement(el);
  }
}

void BlockLayout::layoutElement(const Element *element) {
  if (!element)
    return;

  for (const auto &child : element->children()) {
    layoutNode(child.get());
  }
}

void BlockLayout::layoutText(const std::string &text,
                             const Element *parent_element) {
  auto words = utils::splitWords(text);
  for (const auto &w : words) {
    if (w == "\n") {
      flush();
      continue;
    }
    word(w, parent_element);
  }
}

void BlockLayout::word(const std::string &word_text,
                       const Element *parent_element) {
  TTF_Font *font = currentFont(parent_element);
  if (!font)
    return;

  int w = 0;
  int h = 0;
  TTF_SizeUTF8(font, word_text.c_str(), &w, &h);

  if (cursor_x_ + w > width) {
    flush();
  }

  SDL_Color current_color = {0, 0, 0, 255}; // Default black
  if (parent_element) {
    auto styles = parent_element->style();
    if (styles.find("color") != styles.end()) {
      std::string c_str = styles.at("color");
      if (c_str == "red") current_color = {255, 0, 0, 255};
      else if (c_str == "green") current_color = {0, 128, 0, 255}; // web green is darker, let's just use 128 or 255.
      else if (c_str == "blue") current_color = {0, 0, 255, 255};
      else if (c_str == "yellow") current_color = {255, 255, 0, 255};
      else if (c_str == "white") current_color = {255, 255, 255, 255};
      else if (c_str == "black") current_color = {0, 0, 0, 255};
    }
  }

  line_.push_back({cursor_x_, word_text, font, current_color});

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

    display_list_.push_back({abs_x, abs_y, item.text, item.font, item.color});
  }

  cursor_y_ = baseline + static_cast<int>(1.25 * max_descent);
  cursor_x_ = 0;
  line_.clear();
}

TTF_Font *BlockLayout::currentFont(const Element *element) {
  std::string font_path =
      std::string(ASSETS_DIR) + "/fonts/NotoSansCJK-Regular.ttc";

  int f_size = 16;
  bool f_bold = false;
  bool f_italic = false;

  if (element) {
    auto styles = element->style();
    if (styles.find("font-size") != styles.end()) {
      std::string fs = styles.at("font-size");
      if (fs.length() > 2 && fs.substr(fs.length() - 2) == "px") {
        try {
          f_size = std::stoi(fs.substr(0, fs.length() - 2));
        } catch (...) {
        }
      }
    }
    if (styles.find("font-weight") != styles.end() &&
        styles.at("font-weight") == "bold") {
      f_bold = true;
    }
    if (styles.find("font-style") != styles.end() &&
        styles.at("font-style") == "italic") {
      f_italic = true;
    }
  }

  FontKey key{f_size, f_bold, f_italic};
  if (font_cache_.contains(key))
    return font_cache_[key];

  TTF_Font *font = TTF_OpenFont(font_path.c_str(), f_size);
  if (!font)
    return nullptr;

  int style = TTF_STYLE_NORMAL;
  if (f_bold)
    style |= TTF_STYLE_BOLD;
  if (f_italic)
    style |= TTF_STYLE_ITALIC;
  TTF_SetFontStyle(font, style);

  font_cache_[key] = font;
  return font;
}

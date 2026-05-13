#include "layout/BlockLayout.h"

#include "layout/InputLayout.h"
#include "layout/LineLayout.h"
#include "layout/TextLayout.h"
#include "lexer/Text.h"
#include "utils/Parser.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
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
                         BlockLayout *previous, gfx::FontManager &font_manager)
    : node_(node), parent_(parent), previous_(previous),
      font_manager_(font_manager), cursor_x_(0) {}

/**
 * Very simple linear-gradient parser.
 * Expected format: linear-gradient(to right, red, blue)
 */
static bool parse_linear_gradient(const std::string &value,
                                  std::string &direction, gfx::Color &color1,
                                  gfx::Color &color2) {
  if (value.find("linear-gradient(") != 0)
    return false;

  size_t start = 16; // length of "linear-gradient("
  size_t end = value.find_last_of(')');
  if (end == std::string::npos || end <= start)
    return false;

  std::string content = value.substr(start, end - start);
  std::stringstream ss(content);
  std::string part;
  std::vector<std::string> parts;

  while (std::getline(ss, part, ',')) {
    // Trim whitespace
    size_t f = part.find_first_not_of(" \t");
    size_t l = part.find_last_not_of(" \t");
    if (f != std::string::npos) {
      parts.push_back(part.substr(f, l - f + 1));
    }
  }

  if (parts.size() < 2)
    return false;

  if (parts.size() == 2) {
    direction = "to bottom";
    color1 = gfx::Color::FromName(parts[0].c_str());
    color2 = gfx::Color::FromName(parts[1].c_str());
  } else {
    direction = parts[0];
    color1 = gfx::Color::FromName(parts[1].c_str());
    color2 = gfx::Color::FromName(parts[2].c_str());
  }

  return true;
}

BlockLayout::BlockLayout(std::vector<const Lexeme *> anonymous_children,
                         LayoutObject *parent, BlockLayout *previous,
                         gfx::FontManager &font_manager)
    : node_(nullptr), parent_(parent), previous_(previous),
      font_manager_(font_manager),
      anonymous_children_(std::move(anonymous_children)), cursor_x_(0) {}

float BlockLayout::get_opacity() const {
  if (const auto *el = dynamic_cast<const Element *>(node_)) {
    auto styles = el->style();
    if (styles.find("opacity") != styles.end()) {
      try {
        return std::stof(styles.at("opacity"));
      } catch (...) {
        return 1.0f;
      }
    }
  }
  return 1.0f;
}

std::string BlockLayout::get_blend_mode() const {
  if (const auto *el = dynamic_cast<const Element *>(node_)) {
    auto styles = el->style();
    if (styles.find("mix-blend-mode") != styles.end()) {
      return styles.at("mix-blend-mode");
    }
  }
  return "";
}

bool BlockLayout::is_overflow_clip() const {
  if (const auto *el = dynamic_cast<const Element *>(node_)) {
    auto styles = el->style();
    if (styles.find("overflow") != styles.end() &&
        styles.at("overflow") == "clip") {
      return true;
    }
  }
  return false;
}

float BlockLayout::get_border_radius() const {
  if (const auto *el = dynamic_cast<const Element *>(node_)) {
    auto styles = el->style();
    if (styles.find("border-radius") != styles.end()) {
      std::string rad_str = styles.at("border-radius");
      if (rad_str.find("px") != std::string::npos) {
        rad_str = rad_str.substr(0, rad_str.find("px"));
      }
      try {
        return std::stof(rad_str);
      } catch (...) {
        return 0.0f;
      }
    }
  }
  return 0.0f;
}

static bool is_block_node(const Lexeme *node) {
  if (!node)
    return false;
  if (node->type() == LexemeType::Text)
    return false;
  if (const auto *el = dynamic_cast<const Element *>(node)) {
    return BLOCK_ELEMENTS.contains(el->tag());
  }
  return false;
}

BlockLayout::LayoutMode BlockLayout::layout_mode() const {
  if (!node_) {
    if (!anonymous_children_.empty())
      return LayoutMode::Inline;
    return LayoutMode::Block;
  }

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

  if (!el->children().empty() || el->tag() == "input" || el->tag() == "button")
    return LayoutMode::Inline;

  return LayoutMode::Block;
}

void BlockLayout::layout() {
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
      std::vector<const Lexeme *> inline_run;

      auto flush_inline_run = [&]() {
        if (!inline_run.empty()) {
          auto next = std::make_unique<BlockLayout>(inline_run, this, prev,
                                                    font_manager_);
          prev = next.get();
          children_.push_back(std::move(next));
          inline_run.clear();
        }
      };

      for (const auto &child : el->children()) {
        const Lexeme *child_node = child.get();
        if (child_node->type() == LexemeType::Element) {
          const auto *child_el = dynamic_cast<const Element *>(child_node);
          std::string tag = child_el->tag();
          if (tag == "head" || tag == "script" || tag == "style" ||
              tag == "meta" || tag == "link") {
            continue;
          }
        }

        if (is_block_node(child_node)) {
          flush_inline_run();
          auto next = std::make_unique<BlockLayout>(child_node, this, prev,
                                                    font_manager_);
          prev = next.get();
          children_.push_back(std::move(next));
        } else {
          inline_run.push_back(child_node);
        }
      }
      flush_inline_run();
    }
  } else {
    cursor_x_ = 0;
    new_line();
    if (!anonymous_children_.empty()) {
      for (const auto *n : anonymous_children_) {
        recurse(n);
      }
    } else {
      recurse(node_);
    }
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
    int total = 0;
    for (const auto &child : children_) {
      total += child->height;
    }
    height = total;
  }
}

void BlockLayout::paint(std::vector<std::unique_ptr<DrawCommand>> &out) const {
  const auto *el = dynamic_cast<const Element *>(node_);
  if (el) {
    auto styles = el->style();

    // Support for Rounded Corners (Chapter 11, Section 4)
    float radius = 0.0f;
    if (styles.find("border-radius") != styles.end()) {
      std::string rad_str = styles.at("border-radius");
      // Basic px-stripping (e.g. "10px" -> "10")
      if (rad_str.find("px") != std::string::npos) {
        rad_str = rad_str.substr(0, rad_str.find("px"));
      }
      try {
        radius = std::stof(rad_str);
      } catch (...) {
        radius = 0.0f; // Parsing error fallback
      }
    }

    if (styles.find("box-shadow") != styles.end()) {
      std::string shadow_str = styles.at("box-shadow");
      std::stringstream ss(shadow_str);
      std::string dx_str, dy_str, blur_str, color_str;
      if (ss >> dx_str >> dy_str >> blur_str >> color_str) {
        try {
          int dx = std::stoi(dx_str);
          int dy = std::stoi(dy_str);
          int blur = std::stoi(blur_str);
          gfx::Color color = gfx::Color::FromName(color_str.c_str());
          out.push_back(std::make_unique<DrawBoxShadow>(
              Rect{x, y, (int)width, (int)height}, (float)blur, dx, dy, color));
        } catch (...) {
        }
      }
    }

    if (styles.find("background-color") != styles.end()) {
      std::string bgcolor = styles.at("background-color");
      if (bgcolor != "transparent" && !bgcolor.empty()) {
        gfx::Color color = gfx::Color::FromName(bgcolor.c_str());

        if (radius > 0.0f) {
          out.push_back(std::make_unique<DrawRoundedRect>(
              Rect{x, y, (int)width, (int)height}, radius, color));
        } else {
          out.push_back(
              std::make_unique<DrawRect>(x, y, x + width, y + height, color));
        }
      }
    }

    if (styles.find("background") != styles.end()) {
      std::string bg = styles.at("background");
      std::string dir;
      gfx::Color c1, c2;
      if (parse_linear_gradient(bg, dir, c1, c2)) {
        out.push_back(std::make_unique<DrawLinearGradient>(
            Rect{x, y, (int)width, (int)height}, c1, c2, dir));
      }
    }
  }
}

void BlockLayout::recurse(const Lexeme *node) { layoutNode(node); }

void BlockLayout::layoutNode(const Lexeme *node) {
  if (!node)
    return;

  if (node->type() == LexemeType::Text) {
    const Element *parent_el = nullptr;
    if (const auto *t = dynamic_cast<const Text *>(node)) {
      parent_el = dynamic_cast<const Element *>(t->parent());
    }
    layoutText(node, node->text(), parent_el);
    return;
  }

  if (node->type() == LexemeType::Element) {
    const auto *el = dynamic_cast<const Element *>(node);
    if (el) {
      if (el->tag() == "br") {
        new_line();
      } else if (el->tag() == "input" || el->tag() == "button") {
        input(el);
        return;
      }
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

void BlockLayout::layoutText(const Lexeme *text_node, const std::string &text,
                             const Element *parent_element) {
  auto words = utils::splitWords(text);
  for (const auto &w : words) {
    if (w == "\n") {
      continue;
    }
    word(text_node, w, parent_element);
  }
}

void BlockLayout::word(const Lexeme *node, const std::string &word_text,
                       const Element *parent_element) {
  std::shared_ptr<gfx::Font> font = currentFont(parent_element);
  if (!font)
    return;

  int w = 0;
  int h = 0;
  font->measure_text(word_text, w, h);

  if (cursor_x_ + w > width) {
    new_line();
  }

  gfx::Color current_color = gfx::Color::Black();
  if (parent_element) {
    auto styles = parent_element->style();
    if (styles.find("color") != styles.end()) {
      current_color = gfx::Color::FromName(styles.at("color").c_str());
    }
  }

  // Add the text layout to the current line
  if (!children_.empty()) {
    LineLayout *current_line =
        dynamic_cast<LineLayout *>(children_.back().get());
    if (current_line) {
      auto text_layout =
          std::make_unique<TextLayout>(node, word_text, font, current_color);
      int space_w = 0;
      int space_h = 0;
      font->measure_text(" ", space_w, space_h);

      text_layout->x = this->x + cursor_x_;
      current_line->children_.push_back(std::move(text_layout));
      cursor_x_ += w + space_w;
    }
  }
}

void BlockLayout::new_line() {
  cursor_x_ = 0;
  LayoutObject *prev_line = nullptr;
  if (!children_.empty()) {
    prev_line = children_.back().get();
  }

  children_.push_back(std::make_unique<LineLayout>(node_, this, prev_line));
}

void BlockLayout::input(const Lexeme *node) {
  const auto *el = dynamic_cast<const Element *>(node);
  if (!el)
    return;

  int w = 200; // Match DEFAULT_INPUT_WIDTH in InputLayout
  if (cursor_x_ + w > width) {
    new_line();
  }

  if (!children_.empty()) {
    LineLayout *current_line =
        dynamic_cast<LineLayout *>(children_.back().get());
    if (current_line) {
      std::shared_ptr<gfx::Font> font = currentFont(el);

      gfx::Color color = gfx::Color::Black();
      auto styles = el->style();
      if (styles.count("color")) {
        color = gfx::Color::FromName(styles.at("color").c_str());
      }

      LayoutObject *prev_obj = nullptr;
      if (!current_line->children_.empty()) {
        prev_obj = current_line->children_.back().get();
      }

      auto input_layout = std::make_unique<InputLayout>(node, current_line,
                                                        prev_obj, font, color);
      current_line->children_.push_back(std::move(input_layout));

      int space_w = 0;
      int space_h = 0;
      if (font) {
        font->measure_text(" ", space_w, space_h);
      }
      cursor_x_ += w + space_w;
    }
  }
}

std::shared_ptr<gfx::Font> BlockLayout::currentFont(const Element *element) {
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

  auto font = font_manager_.get_font("Ubuntu", f_size, f_bold, f_italic);
  font_cache_[key] = font;
  return font;
}

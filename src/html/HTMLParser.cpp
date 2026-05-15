#include "HTMLParser.h"
#include "lexer/Text.h"
#include "utils/StringUtils.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string_view>

namespace {

// Check if string s starts with the given prefix.
// Example: starts_with("!doctype", "!") == true
bool starts_with(std::string_view s, std::string_view prefix) {
  return s.size() >= prefix.size() && s.substr(0, prefix.size()) == prefix;
}

// List of tags that close themselves and never have children.
// Example: <br> or <img src="...">
constexpr std::string_view SELF_CLOSING_TAGS[] = {
    "area",  "base", "br",   "col",   "embed",  "hr",    "img",
    "input", "link", "meta", "param", "source", "track", "wbr",
};

// Tags that normally live inside the <head> element.
// Example: <meta>, <title>, <style>
constexpr std::string_view HEAD_TAGS[] = {
    "base", "basefont", "bgsound", "noscript", "link",
    "meta", "title",    "style",   "script",
};

// Return true if the tag is in the self-closing list above.
// Example: is_self_closing("br") == true
bool is_self_closing(std::string_view tag) {
  for (auto t : SELF_CLOSING_TAGS) {
    if (t == tag) {
      return true;
    }
  }
  return false;
}

// Return true if the tag belongs in the document <head>.
// Example: is_head_tag("meta") == true
bool is_head_tag(std::string_view tag) {
  for (auto t : HEAD_TAGS) {
    if (t == tag) {
      return true;
    }
  }
  return false;
}

} // namespace

// Store the full HTML source string that we will parse.
// Example: HTMLParser("<p>Hello</p>")
HTMLParser::HTMLParser(std::string body) : body_(std::move(body)) {}

// Main entry point: walk over the HTML text and split it into
// text and tag chunks, then build a tree of Element/Text nodes.
// Example: "<p>Hi</p>" -> Element("html") -> Element("body") -> Element("p") +
// Text("Hi")
std::unique_ptr<Element> HTMLParser::parse() {
  std::string text;
  bool in_tag = false;

  for (size_t i = 0; i < body_.size(); ++i) {
    char c = body_[i];

    // Special handling for script/style tags: consume everything until the
    // closing tag.
    if (!unfinished_.empty() && (unfinished_.back()->tag() == "script" ||
                                 unfinished_.back()->tag() == "style")) {
      std::string close_tag = "</" + unfinished_.back()->tag() + ">";
      if (i + close_tag.size() <= body_.size()) {
        std::string sub = utils::to_lower(body_.substr(i, close_tag.size()));
        if (sub == close_tag) {
          if (!text.empty()) {
            add_text(text);
            text.clear();
          }

          // Manually close the tag since we are skipping the normal add_tag
          // flow
          std::string raw_closing_tag = "/" + unfinished_.back()->tag();
          add_tag(raw_closing_tag);

          i += close_tag.size() - 1; // Skip the entire </script> or </style>
          continue;
        }
      }
      text += c;
      continue;
    }

    if (c == '<' && !in_tag) {
      in_tag = true;
      if (!text.empty()) {
        add_text(text);
      }
      text.clear();
    } else if (c == '>' && in_tag) {
      in_tag = false;
      add_tag(text);
      text.clear();
    } else {
      text += c;
    }
  }

  if (!in_tag && !text.empty()) {
    add_text(text);
  }

  return finish();
}

// Take the raw text inside a tag and split it into the tag name
// and a map of attributes.
// Example: 'a href="https://example.com"' -> ("a", {"href":
// "https://example.com"})
std::pair<std::string, Element::AttributeMap>
HTMLParser::get_attributes(const std::string &text) const {
  if (text.empty()) {
    return {"", {}};
  }

  size_t i = 0;
  while (i < text.size() &&
         !std::isspace(static_cast<unsigned char>(text[i]))) {
    i++;
  }
  std::string tag = utils::to_lower(text.substr(0, i));

  Element::AttributeMap attributes;

  while (i < text.size()) {
    while (i < text.size() &&
           std::isspace(static_cast<unsigned char>(text[i]))) {
      i++;
    }
    if (i >= text.size())
      break;

    size_t name_start = i;
    while (i < text.size() && text[i] != '=' &&
           !std::isspace(static_cast<unsigned char>(text[i]))) {
      i++;
    }
    std::string key = utils::to_lower(text.substr(name_start, i - name_start));

    while (i < text.size() &&
           std::isspace(static_cast<unsigned char>(text[i]))) {
      i++;
    }

    if (i < text.size() && text[i] == '=') {
      i++; // skip '='
      while (i < text.size() &&
             std::isspace(static_cast<unsigned char>(text[i]))) {
        i++;
      }

      if (i < text.size() && (text[i] == '"' || text[i] == '\'')) {
        char quote = text[i];
        i++;
        size_t val_start = i;
        while (i < text.size() && text[i] != quote) {
          i++;
        }
        attributes[key] = text.substr(val_start, i - val_start);
        if (i < text.size())
          i++; // skip closing quote
      } else {
        size_t val_start = i;
        while (i < text.size() &&
               !std::isspace(static_cast<unsigned char>(text[i]))) {
          i++;
        }
        attributes[key] = text.substr(val_start, i - val_start);
      }
    } else {
      attributes[key] = "";
    }
  }

  return {tag, attributes};
}

// Add a Text node under the most recent unfinished Element.
// Ignores pure whitespace. Example body text between tags becomes Text nodes.
void HTMLParser::add_text(const std::string &text) {
  bool all_space = true;
  for (unsigned char c : text) {
    if (!std::isspace(c)) {
      all_space = false;
      break;
    }
  }
  if (all_space) {
    return;
  }

  implicit_tags(std::nullopt);

  Element *parent = unfinished_.empty() ? nullptr : unfinished_.back().get();
  if (!parent) {
    return;
  }

  parent->appendChild(std::make_unique<Text>(text, parent));
}

// Handle one tag string and update the unfinished stack and tree.
// Supports open tags, close tags like </p>, and self-closing tags like <br>.
void HTMLParser::add_tag(const std::string &tag_text) {
  auto [tag, attributes] = get_attributes(tag_text);
  if (tag.empty()) {
    return;
  }

  if (starts_with(tag, "!")) {
    return;
  }

  implicit_tags(tag);

  if (starts_with(tag, "/")) {
    if (unfinished_.size() == 1) {
      return;
    }

    auto node = std::move(unfinished_.back());
    unfinished_.pop_back();

    Element *parent = unfinished_.back().get();
    node->setParent(parent);
    parent->appendChild(std::move(node));
    return;
  }

  Element *parent = unfinished_.empty() ? nullptr : unfinished_.back().get();

  if (is_self_closing(tag)) {
    if (!parent) {
      return;
    }
    parent->appendChild(
        std::make_unique<Element>(tag, std::move(attributes), parent));
    return;
  }

  unfinished_.push_back(
      std::make_unique<Element>(tag, std::move(attributes), parent));
}

// Insert missing structural tags like <html>, <head>, and <body>
// based on what is currently open and what tag we are about to handle.
// Example: seeing "<p>" first will implicitly create "<html><body>" around it.
void HTMLParser::implicit_tags(const std::optional<std::string> &tag) {
  while (true) {
    std::vector<std::string_view> open_tags;
    open_tags.reserve(unfinished_.size());
    for (const auto &node : unfinished_) {
      open_tags.push_back(node->tag());
    }

    const std::string_view t =
        tag ? std::string_view(*tag) : std::string_view("");

    if (open_tags.empty() && t != "html") {
      add_tag("html");
    } else if (open_tags.size() == 1 && open_tags[0] == "html" && t != "head" &&
               t != "body" && t != "/html") {
      if (is_head_tag(t)) {
        add_tag("head");
      } else {
        add_tag("body");
      }
    } else if (open_tags.size() == 2 && open_tags[0] == "html" &&
               open_tags[1] == "head" && t != "/head" && !is_head_tag(t)) {
      add_tag("/head");
    } else {
      break;
    }
  }
}

// Close any remaining open tags on the unfinished stack and return
// the root Element of the finished tree.
// Example: for "<html><body><p>Hi", this will connect <p> inside <body> inside
// <html>.
std::unique_ptr<Element> HTMLParser::finish() {
  if (unfinished_.empty()) {
    implicit_tags(std::nullopt);
  }

  while (unfinished_.size() > 1) {
    auto node = std::move(unfinished_.back());
    unfinished_.pop_back();

    Element *parent = unfinished_.back().get();
    node->setParent(parent);
    parent->appendChild(std::move(node));
  }

  if (unfinished_.empty()) {
    return nullptr;
  }

  auto root = std::move(unfinished_.back());
  unfinished_.pop_back();
  return root;
}

// Pretty-print the tree starting at the given node.
// Each line shows one node, indented to make the parent/child
// structure visible
// Example:
//   <html>
//     <body>
//       <p>
//         'Hello'
void HTMLParser::print_tree(const Element &node, int indent) {
  std::cout << std::string(indent, ' ') << node.get_string() << '\n';

  for (const auto &child : node.children()) {
    if (child->type() == LexemeType::Element) {
      const auto *el = dynamic_cast<const Element *>(child.get());
      if (el) {
        print_tree(*el, indent + 2);
      }
    } else if (child->type() == LexemeType::Text) {
      const auto *txt = dynamic_cast<const Text *>(child.get());
      if (txt) {
        std::cout << std::string(indent + 2, ' ') << txt->get_string() << '\n';
      }
    }
  }
}

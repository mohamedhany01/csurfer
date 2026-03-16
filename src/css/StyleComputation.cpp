#include "StyleComputation.h"
#include "CSSParser.h"

namespace CSS {

static const std::unordered_map<std::string, std::string> INHERITED_PROPERTIES =
    {
        {"font-size", "16px"},
        {"font-style", "normal"},
        {"font-weight", "normal"},
        {"color", "black"},
};

void computeStyle(Element *node, const std::vector<CSSRule> &rules) {
  if (!node)
    return;

  // 1. Inheritance
  for (const auto &[property, default_value] : INHERITED_PROPERTIES) {
    if (node->parent()) {
      auto parent_style = node->parent()->style();
      if (parent_style.find(property) != parent_style.end()) {
        node->addStyle(property, parent_style[property]);
      } else {
        node->addStyle(property, default_value);
      }
    } else {
      node->addStyle(property, default_value);
    }
  }

  // 2. Cascading Rules
  for (const auto &rule : rules) {
    if (rule.selector && rule.selector->matches(node)) {
      for (const auto &[property, value] : rule.declarations) {
        node->addStyle(property, value);
      }
    }
  }

  // 3. Inline Styles
  auto attrs = node->attributes();
  if (attrs.find("style") != attrs.end()) {
    try {
      CSSParser parser(attrs["style"]);
      // We use a dummy selector just to reuse the parser's body() logic via
      // parse() Wait, CSSParser::parse expects a full rule `selector { body }`.
      // For inline style, we just want to parse the body.
      // Actually, CSSParser doesn't expose `body()` publicly. Let's wrap it in
      // a dummy bracket.
      std::string dummy_css = "dummy {" + attrs["style"] + "}";
      CSSParser inlineParser(dummy_css);
      auto inlineRules = inlineParser.parse();
      if (!inlineRules.empty()) {
        for (const auto &[property, value] : inlineRules[0].declarations) {
          node->addStyle(property, value);
        }
      }
    } catch (...) {
      // Ignore malformed inline styles
    }
  }

  // 4. Compute relative values (Percentages for font-size)
  auto current_style = node->style();
  if (current_style.find("font-size") != current_style.end()) {
    std::string font_size = current_style["font-size"];
    if (!font_size.empty() && font_size.back() == '%') {
      std::string parent_font_size = INHERITED_PROPERTIES.at("font-size");
      if (node->parent()) {
        auto p_style = node->parent()->style();
        if (p_style.find("font-size") != p_style.end()) {
          parent_font_size = p_style["font-size"];
        }
      }

      try {
        double pct =
            std::stod(font_size.substr(0, font_size.length() - 1)) / 100.0;
        // remove "px" from parent string
        double parent_px = 16.0;
        if (parent_font_size.length() > 2 &&
            parent_font_size.substr(parent_font_size.length() - 2) == "px") {
          parent_px = std::stod(
              parent_font_size.substr(0, parent_font_size.length() - 2));
        }

        std::string computed_size = std::to_string(pct * parent_px) + "px";
        node->addStyle("font-size", computed_size);
      } catch (...) {
        // Ignore conversion errors
      }
    }
  }

  // 5. Recursion
  for (const auto &child : node->children()) {
    if (auto *child_elem = dynamic_cast<Element *>(child.get())) {
      computeStyle(child_elem, rules);
    }
  }
}

} // namespace CSS

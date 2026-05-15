#include "StyleComputation.h"
#include "CSSParser.h"
#include "config/Config.h"
#include "utils/StringUtils.h"
#include <sstream>

namespace CSS {

/**
 * Story: A map of CSS properties that are inherited by children if not
 * specified. Use-case: Standard CSS inheritance rules for fonts and colors.
 */
static const std::unordered_map<std::string, std::string> INHERITED_PROPERTIES =
    {
        {"font-size", std::to_string(config::DEFAULT_FONT_SIZE) + "px"},
        {"font-style", "normal"},
        {"font-weight", "normal"},
        {"color", "black"},
};

void compute_style(Element *current_element,
                   const std::vector<CSSRule> &all_rules) {
  if (!current_element)
    return;

  // 1. Inheritance Logic
  for (const auto &[property, default_value] : INHERITED_PROPERTIES) {
    if (current_element->parent()) {
      auto parent_style = current_element->parent()->style();
      if (parent_style.find(property) != parent_style.end()) {
        current_element->add_style(property, parent_style[property]);
      } else {
        current_element->add_style(property, default_value);
      }
    } else {
      current_element->add_style(property, default_value);
    }
  }

  // 2. Cascading Rules (Applying matched selectors)
  for (const auto &rule : all_rules) {
    if (rule.selector && rule.selector->matches(current_element)) {
      for (const auto &[property, value] : rule.declarations) {
        current_element->add_style(property, value);
      }
    }
  }

  // 3. Inline Style Override (Highest priority before !important)
  auto attributes = current_element->attributes();
  if (attributes.count("style")) {
    try {
      // Inline styles are treated as a dummy rule block for parsing reuse
      std::string dummy_css = "dummy {" + attributes.at("style") + "}";
      CSSParser inline_parser(dummy_css);
      auto inline_rules = inline_parser.parse();
      if (!inline_rules.empty()) {
        for (const auto &[property, value] : inline_rules[0].declarations) {
          current_element->add_style(property, value);
        }
      }
    } catch (...) {
      // Silently skip malformed inline styles
    }
  }

  // 4. Resolve Relative Values (e.g., Percentage font-size)
  auto computed_styles = current_element->style();
  if (computed_styles.count("font-size")) {
    std::string size_value = computed_styles.at("font-size");
    if (!size_value.empty() && size_value.back() == '%') {
      std::string parent_size = INHERITED_PROPERTIES.at("font-size");
      if (current_element->parent()) {
        auto parent_styles = current_element->parent()->style();
        if (parent_styles.count("font-size")) {
          parent_size = parent_styles.at("font-size");
        }
      }

      try {
        double percentage =
            std::stod(size_value.substr(0, size_value.length() - 1)) / 100.0;
        double parent_pixels = static_cast<double>(config::DEFAULT_FONT_SIZE);

        if (parent_size.length() > 2 &&
            parent_size.substr(parent_size.length() - 2) == "px") {
          parent_pixels =
              std::stod(parent_size.substr(0, parent_size.length() - 2));
        }

        std::string final_size =
            std::to_string(percentage * parent_pixels) + "px";
        current_element->add_style("font-size", final_size);
      } catch (...) {
        // Fallback to inherited size on conversion failure
      }
    }
  }

  // 5. Recursive descent to children
  for (const auto &child_node : current_element->children()) {
    if (auto *child_element = dynamic_cast<Element *>(child_node.get())) {
      compute_style(child_element, all_rules);
    }
  }
}

} // namespace CSS

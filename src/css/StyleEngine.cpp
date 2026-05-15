#include "StyleEngine.h"

#include "CSSParser.h"
#include "CSSRule.h"
#include "DefaultStyles.h"
#include "StyleComputation.h"
#include "lexer/Element.h"
#include "lexer/Lexeme.h"

#include <algorithm>
#include <functional>
#include <iostream>

StyleEngine::StyleEngine(std::shared_ptr<IRequest> network_engine)
    : network_engine_(std::move(network_engine)) {}

/**
 * Story: Recursively finds all stylesheet links in the DOM.
 */
static std::vector<std::string>
collect_stylesheet_hrefs(const Element *root_element) {
  std::vector<std::string> hrefs;

  std::function<void(const Lexeme *)> walk = [&](const Lexeme *node) {
    if (!node)
      return;
    if (node->type() == LexemeType::Element) {
      const auto *element = dynamic_cast<const Element *>(node);
      if (!element)
        return;

      if (element->tag() == "link") {
        auto attributes = element->attributes();
        if (attributes.count("rel") && attributes.at("rel") == "stylesheet" &&
            attributes.count("href")) {
          hrefs.push_back(attributes.at("href"));
        }
      }

      for (const auto &child : element->children()) {
        walk(child.get());
      }
    }
  };

  walk(root_element);
  return hrefs;
}

/**
 * Story: Sorts CSS rules so that higher-specificity rules appear later.
 * Use-case: This allows the cascade to naturally overwrite earlier rules.
 */
static void sort_rules_by_specificity(std::vector<CSSRule> &rules) {
  std::stable_sort(rules.begin(), rules.end(),
                   [](const CSSRule &a, const CSSRule &b) {
                     int priority_a = a.selector ? a.selector->priority() : 0;
                     int priority_b = b.selector ? b.selector->priority() : 0;
                     return priority_a < priority_b;
                   });
}

void StyleEngine::apply(
    Element *root, const Url &base_url,
    std::function<bool(const Url &, const std::string &)> csp_check,
    const Url &referrer) {
  if (!root)
    return;

  // 1. Parse the built-in browser default styles
  CSSParser default_parser(CSS::DEFAULT_BROWSER_CSS);
  std::vector<CSSRule> all_rules = default_parser.parse();

  // 2. Fetch and parse external stylesheets
  auto hrefs = collect_stylesheet_hrefs(root);
  for (const auto &href : hrefs) {
    try {
      Url style_url = base_url.resolve(href);

      if (csp_check && !csp_check(style_url, "style-src")) {
        std::cout << "[SOP/CSP] Blocked stylesheet from " << style_url.href()
                  << " (CSP Violation)" << std::endl;
        continue;
      }

      std::string css_content =
          network_engine_->request(style_url, "", referrer).body;
      CSSParser external_parser(css_content);
      auto extra_rules = external_parser.parse();
      all_rules.insert(all_rules.end(), extra_rules.begin(), extra_rules.end());
    } catch (const std::exception &error) {
      std::cerr << "[StyleEngine] Failed to load stylesheet: " << href << " — "
                << error.what() << "\n";
    }
  }

  // 3. Sort by specificity for the cascade
  sort_rules_by_specificity(all_rules);

  // 4. Compute final style maps for every element
  CSS::compute_style(root, all_rules);
}

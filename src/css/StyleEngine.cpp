#include "StyleEngine.h"
#include "CSSParser.h"
#include "CSSRule.h"
#include "DefaultStyles.h"
#include "StyleComputation.h"
#include "dom/Element.h"
#include "dom/TreeWalker.h"
#include "utils/Logger.h"

#include <algorithm>
#include <functional>
#include <iostream>

StyleEngine::StyleEngine(std::shared_ptr<IRequest> network_engine)
    : network_engine_(std::move(network_engine)) {}

/**
 * Story: Scans the DOM tree for <link rel="stylesheet"> elements.
 * Use-case: This identifies all external assets that need to be fetched
 * to complete the document's styling.
 */
static std::vector<std::string>
collect_stylesheet_hrefs(const Element *root_element) {
  std::vector<std::string> hrefs;
  auto links = dom::TreeWalker::find_elements(
      const_cast<Element *>(root_element), "link");

  for (auto *element : links) {
    auto attributes = element->attributes();
    if (attributes.count("rel") && attributes.at("rel") == "stylesheet" &&
        attributes.count("href")) {
      hrefs.push_back(attributes.at("href"));
    }
  }

  return hrefs;
}

/**
 * Story: Sorts CSS rules based on their selector specificity.
 * Higher priority rules (e.g. IDs) come later so they naturally
 * override lower priority rules (e.g. tags) in the cascade.
 */
static void sort_rules_by_specificity(std::vector<CSSRule> &rules) {
  std::stable_sort(rules.begin(), rules.end(),
                   [](const CSSRule &a, const CSSRule &b) {
                     int priority_a = a.selector ? a.selector->priority() : 0;
                     int priority_b = b.selector ? b.selector->priority() : 0;
                     return priority_a < priority_b;
                   });
}

void StyleEngine::fetch_external_stylesheets(
    Element *root, const Url &base_url, const Url &referrer,
    std::function<bool(const Url &, const std::string &)> csp_check,
    std::vector<CSSRule> &all_rules) {
  auto hrefs = collect_stylesheet_hrefs(root);
  for (const auto &href : hrefs) {
    try {
      Url style_url = base_url.resolve(href);
      if (csp_check && !csp_check(style_url, "style-src")) {
        CS_LOG_WARN("[SOP/CSP] Blocked stylesheet from {} (CSP Violation)",
                    style_url.href());
        continue;
      }
      std::string css_content =
          network_engine_->request(style_url, "", referrer).body;
      CSSParser external_parser(css_content);
      auto extra_rules = external_parser.parse();

      all_rules.insert(all_rules.end(), extra_rules.begin(), extra_rules.end());
    } catch (const std::exception &error) {
      CS_LOG_ERROR("[StyleEngine] Failed to load stylesheet: {} - {}", href,
                   error.what());
    }
  }
}

/**
 * Story: Main entry point for the styling pipeline.
 * Coordinates defaults, external fetches, sorting, and computation.
 */
void StyleEngine::apply(
    Element *root, const Url &base_url,
    std::function<bool(const Url &, const std::string &)> csp_check,
    const Url &referrer) {
  if (!root)
    return;

  // 1. Load built-in browser default styles (lowest priority)
  CSSParser default_parser(config::DEFAULT_BROWSER_CSS);
  std::vector<CSSRule> all_rules = default_parser.parse();

  // 2. Fetch and parse external stylesheets
  fetch_external_stylesheets(root, base_url, referrer, csp_check, all_rules);

  // 3. Sort by specificity for correct cascading behavior
  sort_rules_by_specificity(all_rules);

  // 4. Compute final style maps for every element in the tree
  CSS::compute_style(root, all_rules);
}

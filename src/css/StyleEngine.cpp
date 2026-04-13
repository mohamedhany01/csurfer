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

StyleEngine::StyleEngine(std::shared_ptr<IRequest> http)
    : http_(std::move(http)) {}

// Recursively collect all <link rel="stylesheet" href="..."> elements.
//
// We walk the entire DOM tree because <link> can theoretically appear
// anywhere (though it's usually in <head>).
//
// Example: finds <link rel="stylesheet" href="style.css"> → returns its href
static std::vector<std::string> collectStylesheetHrefs(const Element *root) {
  std::vector<std::string> hrefs;

  std::function<void(const Lexeme *)> walk = [&](const Lexeme *node) {
    if (!node)
      return;
    if (node->type() == LexemeType::Element) {
      const auto *el = dynamic_cast<const Element *>(node);
      if (!el)
        return;

      if (el->tag() == "link") {
        auto attrs = el->attributes();
        if (attrs.count("rel") && attrs.at("rel") == "stylesheet" &&
            attrs.count("href")) {
          hrefs.push_back(attrs.at("href"));
        }
      }

      for (const auto &child : el->children()) {
        walk(child.get());
      }
    }
  };

  walk(root);
  return hrefs;
}

// Sort CSS rules so lower-specificity rules come first.
// This ensures higher-specificity rules always overwrite lower ones
// when computeStyle iterates through the list in order.
//
// Example:
//   h1 { color: black }     → specificity 1  (comes first)
//   body h1 { color: green} → specificity 2  (overwrites above)
static void sortBySpecificity(std::vector<CSSRule> &rules) {
  std::stable_sort(rules.begin(), rules.end(),
                   [](const CSSRule &a, const CSSRule &b) {
                     int pA = a.selector ? a.selector->priority() : 0;
                     int pB = b.selector ? b.selector->priority() : 0;
                     return pA < pB;
                   });
}

void StyleEngine::apply(
    Element *root, const Url &base_url,
    std::function<bool(const Url &, const std::string &)> csp_check) {
  if (!root)
    return;

  // Step 1: Start with the default browser stylesheet (h1, b, i, etc.)
  CSSParser default_parser(CSS::DEFAULT_BROWSER_CSS);
  std::vector<CSSRule> rules = default_parser.parse();

  // Step 2: Find and fetch external stylesheets from <link> elements
  auto hrefs = collectStylesheetHrefs(root);
  for (const auto &href : hrefs) {
    try {
      Url style_url = base_url.resolve(href);

      // Phase 4: CSP Enforcement
      if (csp_check && !csp_check(style_url, "style-src")) {
        std::cout << "[SOP/CSP] Blocked stylesheet from " << style_url.href()
                  << " (CSP Violation)" << std::endl;
        continue;
      }

      std::string css_text = http_->request(style_url).body;
      CSSParser parser(css_text);
      auto extra = parser.parse();
      rules.insert(rules.end(), extra.begin(), extra.end());
    } catch (const std::exception &e) {
      std::cerr << "[StyleEngine] Failed to load stylesheet: " << href << " — "
                << e.what() << "\n";
    }
  }

  // Step 3: Sort all rules by specificity (ascending)
  sortBySpecificity(rules);

  // Step 4: Walk the DOM and compute each element's final style map
  CSS::computeStyle(root, rules);
}

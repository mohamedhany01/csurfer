#pragma once

#include "request/IRequest.h"
#include "url/Url.h"
#include <functional>
#include <memory>

class Element;

/**
 * Story: The orchestrator for the CSS Styling Pipeline.
 *
 * Use-case: It coordinates the entire process of styling a document:
 * 1. Loading default browser styles.
 * 2. Fetching external stylesheets via the network.
 * 3. Sorting rules by specificity.
 * 4. Triggering the recursive style computation for the DOM tree.
 */
class StyleEngine {
public:
  explicit StyleEngine(std::shared_ptr<IRequest> network_engine);

  /**
   * Story: Applies all relevant CSS styles to the DOM tree.
   *
   * root: The top-level element (usually <html>).
   * base_url: Used to resolve relative paths for external stylesheets.
   * csp_check: Security callback to verify if a stylesheet source is allowed.
   */
  void apply(
      Element *root, const Url &base_url,
      std::function<bool(const Url &, const std::string &)> csp_check = nullptr,
      const Url &referrer = {});

private:
  void fetch_external_stylesheets(
      Element *root, const Url &base_url, const Url &referrer,
      std::function<bool(const Url &, const std::string &)> csp_check,
      std::vector<CSSRule> &all_rules);

  std::shared_ptr<IRequest> network_engine_;
};

#pragma once

#include "request/IRequest.h"
#include "url/Url.h"
#include <functional>
#include <memory>

class Element;

// StyleEngine — the single entry point for all CSS processing.
//
// Encapsulates the entire CSS pipeline so the Browser doesn't need
// to know how CSS works internally. Just call apply() after parsing HTML.
//
// The pipeline (all hidden from the caller):
//   1. Parse the default browser stylesheet
//   2. Find <link rel="stylesheet"> elements in the DOM
//   3. Fetch and parse each external stylesheet via http
//   4. Sort all rules by selector specificity (ascending)
//   5. Walk the DOM and compute each element's final styles
//
// Example:
//   StyleEngine engine(http_client);
//   engine.apply(root_element, page_url);
//
class StyleEngine {
public:
  explicit StyleEngine(std::shared_ptr<IRequest> http);

  // Apply all CSS rules to the DOM tree rooted at `root`.
  // `base_url` is used to resolve relative <link href="..."> paths.
  // `csp_check` is an optional callback to verify if a stylesheet URL is
  // allowed.
  void apply(
      Element *root, const Url &base_url,
      std::function<bool(const Url &, const std::string &)> csp_check = nullptr,
      const Url &referrer = {});

private:
  std::shared_ptr<IRequest> http_;
};

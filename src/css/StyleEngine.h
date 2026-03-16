#pragma once

#include "request/IRequest.h"
#include "url/Url.h"
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
  void apply(Element *root, const Url &base_url);

private:
  std::shared_ptr<IRequest> http_;
};

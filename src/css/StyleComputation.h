#pragma once

#include "../lexer/Element.h"
#include "CSSRule.h"
#include <vector>

namespace CSS {

// Recursively walks the DOM tree starting at 'node'.
// For each element, it applies inherited properties from its parent,
// checks all 'rules' to see which match, and applies inline "style" attributes.
// Finally, computes relative sizes (e.g. 90% font-size -> absolute pixels).
void computeStyle(Element *node, const std::vector<CSSRule> &rules);

} // namespace CSS

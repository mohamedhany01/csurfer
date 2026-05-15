#pragma once

#include "CSSRule.h"
#include "lexer/Element.h"
#include <vector>

namespace CSS {

/**
 * Story: The core logic for computing the final style of a DOM element.
 *
 * Use-case: This resolves the "CSS Cascade" by combining:
 * 1. Inherited properties from parents (e.g., font-family).
 * 2. Matched CSS rules from stylesheets.
 * 3. Inline styles from the 'style' attribute.
 * It also handles unit conversion (e.g., converting '%' to 'px').
 */
void compute_style(Element *element_node,
                   const std::vector<CSSRule> &all_rules);

} // namespace CSS

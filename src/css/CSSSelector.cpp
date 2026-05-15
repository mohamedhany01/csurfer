#include "CSSSelector.h"
#include "dom/Element.h"

TagSelector::TagSelector(std::string tag_name) : tag_(std::move(tag_name)) {}

/**
 * Story: A tag selector matches if the element's tag matches the selector's
 * tag.
 */
bool TagSelector::matches(const Element *element) const {
  return element && element->tag() == tag_;
}

/**
 * Story: Tag selectors have a base priority of 1.
 */
int TagSelector::priority() const { return 1; }

DescendantSelector::DescendantSelector(std::shared_ptr<CSSSelector> ancestor,
                                       std::shared_ptr<CSSSelector> descendant)
    : ancestor_(std::move(ancestor)), descendant_(std::move(descendant)) {}

/**
 * Story: Matches if 'element' matches the descendant selector AND one of its
 * ancestors matches the ancestor selector.
 */
bool DescendantSelector::matches(const Element *element) const {
  if (!element || !descendant_->matches(element)) {
    return false;
  }

  // Walk up the tree to find a matching ancestor
  const Element *current = element->parent();
  while (current) {
    if (ancestor_->matches(current)) {
      return true;
    }
    current = current->parent();
  }

  return false;
}

/**
 * Story: The priority of a descendant selector is the sum of its parts.
 */
int DescendantSelector::priority() const {
  return ancestor_->priority() + descendant_->priority();
}

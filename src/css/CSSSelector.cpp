#include "CSSSelector.h"
#include "../lexer/Element.h"

// TagSelector Implementation
TagSelector::TagSelector(std::string tag)
    : tag_(std::move(tag)), priority_(1) {}

bool TagSelector::matches(const Element *element) const {
  if (!element)
    return false;
  return element->tag() == tag_;
}

int TagSelector::priority() const { return priority_; }

// DescendantSelector Implementation
DescendantSelector::DescendantSelector(std::shared_ptr<CSSSelector> ancestor,
                                       std::shared_ptr<CSSSelector> descendant)
    : ancestor_(std::move(ancestor)), descendant_(std::move(descendant)) {
  priority_ = (ancestor_ ? ancestor_->priority() : 0) +
              (descendant_ ? descendant_->priority() : 0);
}

bool DescendantSelector::matches(const Element *element) const {
  if (!element)
    return false;

  // The target element must first match the descendant selector part
  if (!descendant_ || !descendant_->matches(element)) {
    return false;
  }

  // Traverse upwards to see if any ancestor matches the ancestor selector part
  Element *current = element->parent();
  while (current) {
    if (ancestor_ && ancestor_->matches(current)) {
      return true;
    }
    current = current->parent();
  }
  return false;
}

int DescendantSelector::priority() const { return priority_; }

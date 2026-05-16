#pragma once

#include <memory>
#include <string>

/**
 * Story: The base interface for CSS Selectors (e.g., "div", "p a").
 *
 * Use-case: Used by the Style Engine to determine which CSS rules apply to
 * a given DOM Element. Selectors can be simple (tags) or complex (descendants).
 */
class Element;

class CSSSelector {
public:
  virtual ~CSSSelector() = default;

  /**
   * Story: Checks if the given element matches this selector.
   */
  virtual bool matches(const Element *element) const = 0;

  /**
   * Story: Returns the specificity (priority) of this selector.
   * Higher priority rules overwrite lower ones.
   */
  virtual int priority() const = 0;
};

/**
 * Story: Matches elements by their HTML tag name.
 * Use-case: The most basic selector, e.g., "body { ... }".
 */
class TagSelector : public CSSSelector {
public:
  explicit TagSelector(std::string tag_name);

  bool matches(const Element *element) const override;
  int priority() const override;

private:
  std::string tag_;
};

/**
 * Story: Matches an element if it is a descendant of another matched element.
 * Use-case: Supports nested selection like "div p { ... }".
 */
class DescendantSelector : public CSSSelector {
public:
  DescendantSelector(std::shared_ptr<CSSSelector> ancestor,
                     std::shared_ptr<CSSSelector> descendant);

  bool matches(const Element *element) const override;
  int priority() const override;

private:
  std::shared_ptr<CSSSelector> ancestor_;
  std::shared_ptr<CSSSelector> descendant_;
};

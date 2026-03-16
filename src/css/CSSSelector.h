#pragma once

#include <memory>
#include <string>

// Forward declaration of Element to avoid including full header here
class Element;

class CSSSelector {
public:
  virtual ~CSSSelector() = default;

  // Checks if this selector matches a given Element.
  virtual bool matches(const Element *element) const = 0;

  // Returns the specificity/priority of the selector for cascade sorting.
  virtual int priority() const = 0;
};

// Matches elements by their HTML tag name (e.g., "div", "a").
class TagSelector : public CSSSelector {
public:
  explicit TagSelector(std::string tag);

  bool matches(const Element *element) const override;
  int priority() const override;

private:
  std::string tag_;
  int priority_;
};

// Matches an element if it is a descendant of another matched element (e.g.,
// "div p").
class DescendantSelector : public CSSSelector {
public:
  DescendantSelector(std::shared_ptr<CSSSelector> ancestor,
                     std::shared_ptr<CSSSelector> descendant);

  bool matches(const Element *element) const override;
  int priority() const override;

private:
  std::shared_ptr<CSSSelector> ancestor_;
  std::shared_ptr<CSSSelector> descendant_;
  int priority_;
};

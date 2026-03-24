#include "Element.h"
#include "Text.h"

Element::Element(std::string tag, Element::AttributeMap attributes,
                 Element *parent)
    : tag_(std::move(tag)), attributes_(std::move(attributes)), children_(),
      parent_(parent) {}

LexemeType Element::type() const { return LexemeType::Element; }

const std::string &Element::element() const { return tag_; }

const std::string &Element::tag() const { return tag_; }

const Element::AttributeMap &Element::attributes() const { return attributes_; }

void Element::setAttribute(const std::string &name, const std::string &value) {
  attributes_[name] = value;
}

Element *Element::parent() const { return parent_; }

void Element::setParent(Element *parent) { parent_ = parent; }

const std::vector<std::unique_ptr<Lexeme>> &Element::children() const {
  return children_;
}

void Element::appendChild(std::unique_ptr<Lexeme> child) {
  if (!child) {
    return;
  }

  if (auto *t = dynamic_cast<Text *>(child.get())) {
    t->setParent(this);
  } else if (auto *e = dynamic_cast<Element *>(child.get())) {
    e->setParent(this);
  }

  children_.push_back(std::move(child));
}

void Element::clearChildren() { children_.clear(); }

void Element::moveChildrenFrom(Element *other) {
  for (auto &child : other->children_) {
    if (auto *t = dynamic_cast<Text *>(child.get())) {
      t->setParent(this);
    } else if (auto *e = dynamic_cast<Element *>(child.get())) {
      e->setParent(this);
    }
    children_.push_back(std::move(child));
  }
  other->children_.clear();
}

const Element::StyleMap &Element::style() const { return style_; }

void Element::addStyle(const std::string &property, const std::string &value) {
  style_[property] = value;
}

std::string Element::get_string() const { return "<" + tag_ + ">"; }

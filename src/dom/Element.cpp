#include "Element.h"
#include <iostream>

Element::Element(std::string tag_name, AttributeMap attributes, Element *parent)
    : tag_(std::move(tag_name)), attributes_(std::move(attributes)),
      parent_(parent) {}

LexemeType Element::type() const { return LexemeType::Element; }

const std::string &Element::tag() const { return tag_; }

const Element::AttributeMap &Element::attributes() const { return attributes_; }

void Element::set_attribute(const std::string &name, const std::string &value) {
  attributes_[name] = value;
}

Element *Element::parent() const { return parent_; }

void Element::set_parent(Element *parent_node) { parent_ = parent_node; }

const std::vector<std::unique_ptr<Lexeme>> &Element::children() const {
  return children_;
}

void Element::append_child(std::unique_ptr<Lexeme> child_node) {
  children_.push_back(std::move(child_node));
}

void Element::clear_children() { children_.clear(); }

void Element::move_children_from(Element *source_element) {
  if (!source_element)
    return;
  for (auto &child : source_element->children_) {
    child->set_focused(false); // Reset focus on move
    children_.push_back(std::move(child));
  }
  source_element->children_.clear();
}

const Element::StyleMap &Element::style() const { return style_; }

void Element::add_style(const std::string &property, const std::string &value) {
  style_[property] = value;
}

std::string Element::get_string() const {
  std::string result = "<" + tag_;
  for (const auto &[name, value] : attributes_) {
    result += " " + name + "='" + value + "'";
  }
  result += ">";
  return result;
}

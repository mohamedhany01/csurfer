#include "Text.h"

Text::Text(std::string text_content, Element *parent_element)
    : value_(std::move(text_content)), parent_(parent_element) {}

LexemeType Text::type() const { return LexemeType::Text; }

const std::string &Text::text() const { return value_; }

Element *Text::parent() const { return parent_; }

void Text::set_parent(Element *parent_node) { parent_ = parent_node; }

std::string Text::get_string() const { return "'" + value_ + "'"; }

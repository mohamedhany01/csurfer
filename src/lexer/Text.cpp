#include "Text.h"

Text::Text(std::string text, Element *parent)
    : value_(std::move(text)), parent_(parent) {}

LexemeType Text::type() const { return LexemeType::Text; }

const std::string &Text::text() const { return value_; }

Element *Text::parent() const { return parent_; }

void Text::setParent(Element *parent) { parent_ = parent; }

std::string Text::get_string() const { return "'" + value_ + "'"; }

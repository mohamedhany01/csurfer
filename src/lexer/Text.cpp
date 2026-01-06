#include "Text.h"

Text::Text(std::string value) : value_(std::move(value)) {}

LexemeType Text::type() const { return LexemeType::Text; }

const std::string &Text::text() const { return value_; }

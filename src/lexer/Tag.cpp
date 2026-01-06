#include "Tag.h"

Tag::Tag(std::string name) : name_(std::move(name)) {}

LexemeType Tag::type() const { return LexemeType::Tag; }

const std::string &Tag::tag() const { return name_; }

#pragma once

#include "CSSSelector.h"
#include <memory>
#include <string>
#include <unordered_map>

// Represents a single parsed CSS rule block.
// Example: p { color: blue; font-size: 16px; }
struct CSSRule {
  std::shared_ptr<CSSSelector> selector;
  std::unordered_map<std::string, std::string> declarations;
};

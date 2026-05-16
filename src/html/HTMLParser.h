#pragma once
#include "dom/Element.h"

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

class HTMLParser {
public:
  explicit HTMLParser(std::string body);

  std::unique_ptr<Element> parse();

private:
  std::string body_;
  std::vector<std::unique_ptr<Element>> unfinished_;

  std::pair<std::string, Element::AttributeMap>
  get_attributes(const std::string &text) const;

  void add_text(const std::string &text);
  void add_tag(const std::string &tag_text);
  void implicit_tags(const std::optional<std::string> &tag);
  std::unique_ptr<Element> finish();
};

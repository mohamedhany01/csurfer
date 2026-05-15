#pragma once
#include "Lexeme.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * Story: Represents an HTML Element (a tag like <div> or <a>).
 *
 * Use-case: This is the primary building block of the DOM tree.
 * It holds attributes (id, class, href) and child nodes.
 * It also stores computed styles after the CSS engine has run.
 */
class Element : public Lexeme {
public:
  using AttributeMap = std::unordered_map<std::string, std::string>;
  using StyleMap = std::unordered_map<std::string, std::string>;

  explicit Element(std::string tag_name, AttributeMap attributes = {},
                   Element *parent = nullptr);

  LexemeType type() const override;

  /**
   * Story: Returns the HTML tag name (e.g., "body").
   */
  const std::string &tag() const override;

  /**
   * Story: Returns the attribute map.
   */
  const AttributeMap &attributes() const;

  /**
   * Story: Sets or updates an attribute.
   */
  void set_attribute(const std::string &name, const std::string &value);

  /**
   * Story: Accesses the parent node.
   */
  Element *parent() const override;
  void set_parent(Element *parent_node);

  /**
   * Story: Manages children in the DOM tree.
   */
  const std::vector<std::unique_ptr<Lexeme>> &children() const;
  void append_child(std::unique_ptr<Lexeme> child_node);
  void clear_children();
  void move_children_from(Element *source_element);

  /**
   * Story: Manages computed CSS styles.
   */
  const StyleMap &style() const;
  void add_style(const std::string &property, const std::string &value);

  /**
   * Story: Debug string (e.g., "<div id='foo'>").
   */
  std::string get_string() const override;

private:
  std::string tag_;
  AttributeMap attributes_;
  StyleMap style_;
  std::vector<std::unique_ptr<Lexeme>> children_;
  Element *parent_;
};

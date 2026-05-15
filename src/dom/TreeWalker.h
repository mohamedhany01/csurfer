#pragma once
#include "dom/Element.h"
#include <deque>
#include <functional>
#include <string>
#include <vector>

namespace dom {

/**
 * Story: A utility for traversing the DOM tree.
 *
 * Use-case: Used by JS, CSS, and Layout engines to find specific elements
 * without duplicating traversal boilerplate.
 */
class TreeWalker {
public:
  /**
   * Story: Performs a Breadth-First Search (BFS) starting from the given root.
   * Calls the predicate for each element.
   */
  static std::vector<Element *> bfs(Element *root,
                                    std::function<bool(Element *)> predicate) {
    std::vector<Element *> results;
    if (!root) {
      return results;
    }

    std::deque<Element *> queue = {root};

    while (!queue.empty()) {
      Element *element = queue.front();
      queue.pop_front();

      if (!element) {
        continue;
      }

      if (predicate(element)) {
        results.push_back(element);
      }

      for (auto &child : element->children()) {
        if (child->type() == LexemeType::Element) {
          queue.push_back(static_cast<Element *>(child.get()));
        }
      }
    }
    return results;
  }

  /**
   * Story: Finds all elements matching a simple tag name or #id selector.
   */
  static std::vector<Element *> find_elements(Element *root,
                                              const std::string &selector) {
    return bfs(root, [&](Element *element) {
      if (selector.starts_with("#")) {
        auto it = element->attributes().find("id");
        return it != element->attributes().end() &&
               it->second == selector.substr(1);
      }
      return element->tag() == selector;
    });
  }
};

} // namespace dom

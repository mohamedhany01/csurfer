#include "browser/TabInputHandler.h"
#include "browser/TabNavigator.h"
#include "browser/TabRenderer.h"
#include "config/Config.h"
#include "dom/Element.h"
#include "js/JSContext.h"
#include "layout/DocumentLayout.h"
#include "layout/LayoutTree.h"
#include "utils/Logger.h"
#include "utils/Parser.h"
#include "utils/StringUtils.h"
#include <algorithm>

TabInputHandler::TabInputHandler(TabRenderer &renderer, TabNavigator &navigator)
    : renderer_(renderer), navigator_(navigator) {}

std::optional<std::pair<Url, std::string>>
TabInputHandler::handle_mousedown(int x, int y, DocumentLayout *document_layout,
                                  int window_width,
                                  JSContext *javascript_context) {
  if (!document_layout)
    return std::nullopt;

  int ui_height = config::UI_HEIGHT;
  int viewport_height = config::WINDOW_HEIGHT - ui_height;
  int document_height = (int)document_layout->bounds().height + 100;

  if (document_height > viewport_height &&
      x >= window_width - config::SCROLLBAR_WIDTH) {
    renderer_.set_dragging_scrollbar(true);
    handle_mousemove(x, y, document_layout);
    return std::nullopt;
  }

  return click(x, y, document_layout, javascript_context);
}

void TabInputHandler::handle_mousemove(int x, int y,
                                       const DocumentLayout *document_layout) {
  if (!renderer_.is_dragging_scrollbar() || !document_layout)
    return;

  int ui_height = config::UI_HEIGHT;
  int viewport_height = config::WINDOW_HEIGHT - ui_height;
  int document_height = (int)document_layout->bounds().height + 100;

  double scroll_ratio = (double)y / viewport_height;
  int max_scroll = std::max(0, document_height - viewport_height);
  renderer_.set_scroll(
      std::min(max_scroll, std::max(0, (int)(scroll_ratio * document_height -
                                             viewport_height / 2))));
}

void TabInputHandler::handle_mouseup(int /*x*/, int /*y*/) {
  renderer_.set_dragging_scrollbar(false);
}

void TabInputHandler::handle_keypress(SDL_Keycode key, const std::string &text,
                                      DocumentLayout *document_layout,
                                      JSContext *javascript_context) {
  if (!focused_element_)
    return;

  auto *element = dynamic_cast<Element *>(focused_element_);
  if (!element)
    return;

  if (javascript_context &&
      javascript_context->dispatch_event("keydown", element)) {
    return;
  }

  std::string value_string = element->attributes().count("value")
                                 ? element->attributes().at("value")
                                 : "";

  if (key == SDLK_BACKSPACE) {
    if (!value_string.empty()) {
      value_string.pop_back();
    }
  } else if (!text.empty()) {
    value_string += text;
  }

  element->set_attribute("value", value_string);

  if (document_layout) {
    document_layout->layout();
    renderer_.rebuild_display_list(*document_layout);
  }
}

std::optional<std::pair<Url, std::string>>
TabInputHandler::click(int x, int y, DocumentLayout *document_layout,
                       JSContext *javascript_context) {
  if (!document_layout)
    return std::nullopt;

  auto total_y = y + renderer_.current_scroll();

  auto all_nodes = tree_to_list(*document_layout);
  const Lexeme *clicked_node = nullptr;

  for (auto it = all_nodes.rbegin(); it != all_nodes.rend(); ++it) {
    const LayoutObject *object = *it;
    if (object->bounds().contains({x, total_y})) {
      clicked_node = object->node();
      if (clicked_node)
        break;
    }
  }

  if (clicked_node) {
    if (focused_element_) {
      focused_element_->set_focused(false);
    }
    focused_element_ = nullptr;

    const Lexeme *current_lexeme = clicked_node;
    while (current_lexeme) {
      if (current_lexeme->type() == LexemeType::Element) {
        const auto *element = dynamic_cast<const Element *>(current_lexeme);
        if (javascript_context &&
            javascript_context->dispatch_event(
                "click", const_cast<Element *>(element))) {
          return std::nullopt;
        }
        if (element->tag() == "a" && element->attributes().contains("href")) {
          return std::make_pair(
              navigator_.url().resolve(element->attributes().at("href")), "");
        } else if (element->tag() == "input") {
          focused_element_ = const_cast<Element *>(element);
          focused_element_->set_focused(true);
          focused_element_->set_attribute("value", "");

          renderer_.rebuild_display_list(*document_layout);
          return std::nullopt;
        } else if (element->tag() == "button") {
          const Element *form_search = element;
          while (form_search) {
            if (form_search->tag() == "form" &&
                form_search->attributes().contains("action")) {
              return submit_form(form_search, javascript_context);
            }
            form_search = dynamic_cast<const Element *>(form_search->parent());
          }
        }
      }
      current_lexeme = current_lexeme->parent();
    }
  }

  renderer_.rebuild_display_list(*document_layout);
  return std::nullopt;
}

std::optional<std::pair<Url, std::string>>
TabInputHandler::submit_form(const Element *form_element,
                             JSContext *javascript_context) {
  if (!form_element)
    return std::nullopt;

  if (javascript_context &&
      javascript_context->dispatch_event("submit",
                                         const_cast<Element *>(form_element))) {
    return std::nullopt;
  }

  std::string payload_string;

  std::vector<const Lexeme *> queue = {form_element};
  while (!queue.empty()) {
    const Lexeme *node = queue.front();
    queue.erase(queue.begin());

    if (node->type() == LexemeType::Element) {
      const auto *element = dynamic_cast<const Element *>(node);
      if (element->tag() == "input" && element->attributes().contains("name")) {
        std::string name = element->attributes().at("name");
        std::string value = element->attributes().count("value")
                                ? element->attributes().at("value")
                                : "";
        if (!payload_string.empty())
          payload_string += "&";
        payload_string += utils::url_percent_encode(name) + "=" +
                          utils::url_percent_encode(value);
      }
      for (const auto &child : element->children()) {
        queue.push_back(child.get());
      }
    }
  }

  std::string action_url_string = form_element->attributes().at("action");
  CS_LOG_INFO("Submitting form to: {} with payload: {}", action_url_string,
              payload_string);

  return std::make_pair(navigator_.url().resolve(action_url_string),
                        payload_string);
}

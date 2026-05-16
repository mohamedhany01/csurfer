#pragma once

#include "dom/Element.h"
#include "url/Url.h"
#include <SDL2/SDL.h>
#include <memory>
#include <optional>
#include <string>
#include <utility>

class TabRenderer;
class TabNavigator;
class DocumentLayout;
class JSContext;

/**
 * Story: Manages input events and interactions for a Tab.
 *
 * Use-case: Handles click, mouse move, keypress events, and form submissions.
 */
class TabInputHandler {
public:
  TabInputHandler(TabRenderer &renderer, TabNavigator &navigator);

  /**
   * Story: Handles mouse down events.
   * Why: To detect clicks on links, inputs, buttons, or scrollbar.
   */
  std::optional<std::pair<Url, std::string>>
  handle_mousedown(int x, int y, DocumentLayout *document_layout,
                   int window_width, JSContext *javascript_context);

  /**
   * Story: Handles mouse move events.
   * Why: To handle scrollbar dragging.
   */
  void handle_mousemove(int x, int y, const DocumentLayout *document_layout);

  /**
   * Story: Handles mouse up events.
   * Why: To stop scrollbar dragging.
   */
  void handle_mouseup(int x, int y);

  /**
   * Story: Handles key press events.
   * Why: To type into focused input fields.
   */
  void handle_keypress(SDL_Keycode key, const std::string &text,
                       DocumentLayout *document_layout,
                       JSContext *javascript_context);

  std::optional<std::pair<Url, std::string>>
  click(int x, int y, DocumentLayout *document_layout,
        JSContext *javascript_context);

  Element *focused_element() const { return focused_element_; }
  void set_focused_element(Element *element) { focused_element_ = element; }

private:
  TabRenderer &renderer_;
  TabNavigator &navigator_;
  Element *focused_element_ = nullptr;

  std::optional<std::pair<Url, std::string>>
  submit_form(const Element *form_element, JSContext *javascript_context);
};

#pragma once

#include "duktape.h"
#include <string>
#include <unordered_map>
#include <vector>

class Tab;
class Element;

/**
 * Story: The JavaScript execution environment for a single tab.
 *
 * Use-case: It wraps the Duktape engine and provides "Native Bridge"
 * functions that allow JS code to interact with the C++ DOM and
 * network engine.
 */
class JSContext {
public:
  explicit JSContext(Tab *tab_host);
  ~JSContext();

  /**
   * Story: Executes a string of JavaScript code.
   */
  void run(const std::string &script_name, const std::string &code);

  /**
   * Story: Dispatches a DOM event (like 'click') to the JS environment.
   */
  bool dispatch_event(const std::string &event_type, Element *element);

  // Handle Mapping (Story: Bridges C++ pointers to JS integer handles)
  int get_handle(Element *element);
  Element *get_element(int handle);

private:
  // Native Bridge Callbacks
  static duk_ret_t native_print(duk_context *ctx);
  static duk_ret_t native_querySelectorAll(duk_context *ctx);
  static duk_ret_t native_getAttribute(duk_context *ctx);
  static duk_ret_t native_innerHTML_set(duk_context *ctx);
  static duk_ret_t native_XMLHttpRequest_send(duk_context *ctx);
  static duk_ret_t native_cookie_get(duk_context *ctx);
  static duk_ret_t native_cookie_set(duk_context *ctx);

  Tab *tab_;
  duk_context *duktape_context_;

  std::unordered_map<Element *, int> element_to_handle_;
  std::vector<Element *> handle_to_element_;
};

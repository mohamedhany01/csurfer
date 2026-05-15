#pragma once

#include "IJSHost.h"
#include "duktape.h"
#include <string>
#include <unordered_map>
#include <vector>

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
  explicit JSContext(IJSHost *js_host);
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
  static duk_ret_t native_query_selector_all(duk_context *ctx);
  static duk_ret_t native_get_attribute(duk_context *ctx);
  static duk_ret_t native_inner_html_set(duk_context *ctx);
  static duk_ret_t native_xml_http_request_send(duk_context *ctx);
  static duk_ret_t native_cookie_get(duk_context *ctx);
  static duk_ret_t native_cookie_set(duk_context *ctx);

  /**
   * Story: Helper to retrieve the JSContext instance from the Duktape stash.
   */
  static JSContext *get_context(duk_context *ctx);

  IJSHost *host_;
  duk_context *duktape_context_;

  std::unordered_map<Element *, int> element_to_handle_;
  std::vector<Element *> handle_to_element_;
};

#include "JSContext.h"
#include "dom/Element.h"
#include "dom/TreeWalker.h"
#include "request/IRequest.h"
#include "utils/Logger.h"

#include <fstream>

JSContext::JSContext(IJSHost *host) : host_(host) {
  duktape_context_ = duk_create_heap_default();

  // Story: Store 'this' pointer in the global stash for native C callback
  // access
  duk_push_global_stash(duktape_context_);
  duk_push_pointer(duktape_context_, this);
  duk_put_prop_string(duktape_context_, -2, "js_context");
  duk_pop(duktape_context_);

  // Register 'log' (print) function
  duk_push_c_function(duktape_context_, native_print, DUK_VARARGS);
  duk_put_global_string(duktape_context_, "log");

  // Register 'querySelectorAll' function
  duk_push_c_function(duktape_context_, native_query_selector_all,
                      1 /* nargs */);
  duk_put_global_string(duktape_context_, "querySelectorAll");

  // Register 'getAttribute' function
  duk_push_c_function(duktape_context_, native_get_attribute, 2 /* nargs */);
  duk_put_global_string(duktape_context_, "getAttribute");

  // Register 'innerHTML_set' function
  duk_push_c_function(duktape_context_, native_inner_html_set, 2 /* nargs */);
  duk_put_global_string(duktape_context_, "innerHTML_set");

  // Register 'XMLHttpRequest_send' function
  duk_push_c_function(duktape_context_, native_xml_http_request_send,
                      3 /* nargs */);
  duk_put_global_string(duktape_context_, "XMLHttpRequest_send");

  // Register native cookie handlers
  duk_push_c_function(duktape_context_, native_cookie_get, 0);
  duk_put_global_string(duktape_context_, "native_cookie_get");
  duk_push_c_function(duktape_context_, native_cookie_set, 1);
  duk_put_global_string(duktape_context_, "native_cookie_set");

  // Story: Load the browser runtime library
  std::ifstream runtime_file("assets/runtime.js");
  if (runtime_file.is_open()) {
    std::string content((std::istreambuf_iterator<char>(runtime_file)),
                        (std::istreambuf_iterator<char>()));
    run("runtime.js", content);
  }
}

JSContext::~JSContext() { duk_destroy_heap(duktape_context_); }

JSContext *JSContext::get_context(duk_context *ctx) {
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "js_context");
  JSContext *self = static_cast<JSContext *>(duk_get_pointer(ctx, -1));
  duk_pop_2(ctx);
  return self;
}

void JSContext::run(const std::string &script_name, const std::string &code) {
  if (duk_peval_string(duktape_context_, code.c_str()) != 0) {
    CS_LOG_ERROR("[JS] Error in {}: {}", script_name,
                 duk_safe_to_string(duktape_context_, -1));
  }
  duk_pop(duktape_context_);
}

bool JSContext::dispatch_event(const std::string &event_type,
                               Element *element) {
  duk_get_global_string(duktape_context_, "dispatchEvent");
  duk_push_string(duktape_context_, event_type.c_str());
  duk_push_int(duktape_context_, get_handle(element));
  if (duk_pcall(duktape_context_, 2) != 0) {
    CS_LOG_ERROR("[JS] Event Dispatch Error: {}",
                 duk_safe_to_string(duktape_context_, -1));
    duk_pop(duktape_context_);
    return false;
  }
  bool prevent_default = duk_get_boolean(duktape_context_, -1);
  duk_pop(duktape_context_);
  return prevent_default;
}

int JSContext::get_handle(Element *element) {
  if (element_to_handle_.find(element) != element_to_handle_.end()) {
    return element_to_handle_[element];
  }

  int handle = static_cast<int>(handle_to_element_.size());
  element_to_handle_[element] = handle;
  handle_to_element_.push_back(element);
  return handle;
}

Element *JSContext::get_element(int handle) {
  if (handle >= 0 && handle < static_cast<int>(handle_to_element_.size())) {
    return handle_to_element_[handle];
  }
  return nullptr;
}

duk_ret_t JSContext::native_print(duk_context *ctx) {
  CS_LOG_INFO("JS log: {}", duk_safe_to_string(ctx, 0));
  return 0;
}

duk_ret_t JSContext::native_query_selector_all(duk_context *ctx) {
  JSContext *self = get_context(ctx);
  if (!self || !self->host_ || !self->host_->root()) {
    duk_push_array(ctx);
    return 1;
  }

  const char *selector_string = duk_to_string(ctx, 0);
  std::vector<Element *> results = dom::TreeWalker::find_elements(
      dynamic_cast<Element *>(self->host_->root()), selector_string);

  duk_push_array(ctx);
  for (size_t i = 0; i < results.size(); ++i) {
    duk_push_int(ctx, self->get_handle(results[i]));
    duk_put_prop_index(ctx, -2, static_cast<duk_uarridx_t>(i));
  }
  return 1;
}

duk_ret_t JSContext::native_get_attribute(duk_context *ctx) {
  JSContext *self = get_context(ctx);
  int handle = duk_to_int(ctx, 0);
  const char *attribute_name = duk_to_string(ctx, 1);

  Element *element = self->get_element(handle);
  if (element && element->attributes().count(attribute_name)) {
    duk_push_string(ctx, element->attributes().at(attribute_name).c_str());
  } else {
    duk_push_null(ctx);
  }
  return 1;
}

duk_ret_t JSContext::native_inner_html_set(duk_context *ctx) {
  JSContext *self = get_context(ctx);
  int handle = duk_to_int(ctx, 0);
  const char *html_content = duk_to_string(ctx, 1);
  (void)html_content;

  Element *element = self->get_element(handle);
  if (element) {
    // Story: innerHTML setting logic would go here.
  }
  return 0;
}

duk_ret_t JSContext::native_xml_http_request_send(duk_context *ctx) {
  JSContext *self = get_context(ctx);
  if (!self || !self->host_)
    return 0;

  const char *method = duk_to_string(ctx, 0);
  const char *url_string = duk_to_string(ctx, 1);
  const char *request_body = duk_to_string(ctx, 2);

  Url target_url = self->host_->url().resolve(url_string);
  std::string page_origin = self->host_->url().origin();
  std::string target_origin = target_url.origin();

  // Story: Same-Origin Policy (SOP) check
  if (page_origin != target_origin) {
    CS_LOG_WARN("[SOP] Blocked cross-origin request from {} to {}", page_origin,
                target_origin);
    duk_push_string(ctx, "");
    return 1;
  }

  CS_LOG_INFO("[JS XHR] Sending {} request to {}", method, target_url.href());
  auto response =
      self->host_->network_engine()->request(target_url, request_body);

  duk_push_string(ctx, response.body.c_str());
  return 1;
}

duk_ret_t JSContext::native_cookie_get(duk_context *ctx) {
  JSContext *self = get_context(ctx);
  if (!self || !self->host_)
    return 0;

  std::string cookies =
      self->host_->network_engine()->get_cookies(self->host_->url());
  duk_push_string(ctx, cookies.c_str());
  return 1;
}

duk_ret_t JSContext::native_cookie_set(duk_context *ctx) {
  JSContext *self = get_context(ctx);
  if (!self || !self->host_)
    return 0;

  const char *cookie_value = duk_to_string(ctx, 0);
  self->host_->network_engine()->set_cookie(self->host_->url(), cookie_value);
  return 0;
}

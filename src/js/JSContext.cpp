#include "JSContext.h"
#include "browser/Tab.h"
#include "lexer/Element.h"

#include <fstream>
#include <iostream>

JSContext::JSContext(Tab *tab) : tab_(tab) {
  ctx_ = duk_create_heap_default();

  // Store 'this' in global stash for native access
  duk_push_global_stash(ctx_);
  duk_push_pointer(ctx_, this);
  duk_put_prop_string(ctx_, -2, "js_context");
  duk_pop(ctx_);

  // Register 'print' function
  duk_push_c_function(ctx_, native_print, DUK_VARARGS);
  duk_put_global_string(ctx_, "log");

  // Register 'querySelectorAll' function
  duk_push_c_function(ctx_, native_querySelectorAll, 1 /* nargs */);
  duk_put_global_string(ctx_, "querySelectorAll");

  // Register 'getAttribute' function
  duk_push_c_function(ctx_, native_getAttribute, 2 /* nargs */);
  duk_put_global_string(ctx_, "getAttribute");

  // Register 'innerHTML_set' function
  duk_push_c_function(ctx_, native_innerHTML_set, 2 /* nargs */);
  duk_put_global_string(ctx_, "innerHTML_set");

  // Register 'XMLHttpRequest_send' function
  duk_push_c_function(ctx_, native_XMLHttpRequest_send, 3 /* nargs */);
  duk_put_global_string(ctx_, "XMLHttpRequest_send");

  // Load runtime.js
  std::ifstream f("assets/runtime.js");
  if (f.is_open()) {
    std::string content((std::istreambuf_iterator<char>(f)),
                        (std::istreambuf_iterator<char>()));
    run("runtime.js", content);
  }
}

JSContext::~JSContext() { duk_destroy_heap(ctx_); }

void JSContext::run(const std::string &script_name, const std::string &code) {
  if (duk_peval_string(ctx_, code.c_str()) != 0) {
    std::cerr << "[JS] Error in " << script_name << ": "
              << duk_safe_to_string(ctx_, -1) << std::endl;
  }
  duk_pop(ctx_);
}

bool JSContext::dispatch_event(const std::string &type, Element *elt) {
  duk_get_global_string(ctx_, "dispatchEvent");
  duk_push_string(ctx_, type.c_str());
  duk_push_int(ctx_, get_handle(elt));

  if (duk_pcall(ctx_, 2) != 0) {
    std::cerr << "[JS] Event Dispatch Error: " << duk_safe_to_string(ctx_, -1)
              << std::endl;
    duk_pop(ctx_);
    return false;
  }

  bool preventDefault = duk_get_boolean(ctx_, -1);
  duk_pop(ctx_);
  return preventDefault;
}

int JSContext::get_handle(Element *elt) {
  if (element_to_handle_.find(elt) != element_to_handle_.end()) {
    return element_to_handle_[elt];
  }

  int handle = static_cast<int>(handle_to_element_.size());
  element_to_handle_[elt] = handle;
  handle_to_element_.push_back(elt);
  return handle;
}

Element *JSContext::get_element(int handle) {
  if (handle >= 0 && handle < static_cast<int>(handle_to_element_.size())) {
    return handle_to_element_[handle];
  }
  return nullptr;
}

duk_ret_t JSContext::native_print(duk_context *ctx) {
  std::cout << "JS log: " << duk_safe_to_string(ctx, 0) << std::endl;
  return 0;
}

duk_ret_t JSContext::native_querySelectorAll(duk_context *ctx) {
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "js_context");
  JSContext *self = static_cast<JSContext *>(duk_get_pointer(ctx, -1));
  duk_pop_2(ctx);

  const char *selector_str = duk_to_string(ctx, 0);
  std::string selector(selector_str);
  std::vector<Element *> results;

  std::vector<Element *> queue;
  if (self->tab_ && self->tab_->root()) {
    queue.push_back(dynamic_cast<Element *>(self->tab_->root()));
  }

  while (!queue.empty()) {
    Element *el = queue.back();
    queue.pop_back();
    if (!el)
      continue;

    bool match = false;
    if (selector.starts_with("#")) {
      auto it = el->attributes().find("id");
      if (it != el->attributes().end() && it->second == selector.substr(1)) {
        match = true;
      }
    } else if (el->tag() == selector) {
      match = true;
    }

    if (match) {
      results.push_back(el);
    }

    for (auto &child : el->children()) {
      if (child->type() == LexemeType::Element) {
        queue.push_back(static_cast<Element *>(child.get()));
      }
    }
  }

  duk_push_array(ctx);
  for (size_t i = 0; i < results.size(); ++i) {
    duk_push_int(ctx, self->get_handle(results[i]));
    duk_put_prop_index(ctx, -2, static_cast<duk_uarridx_t>(i));
  }
  return 1;
}

duk_ret_t JSContext::native_getAttribute(duk_context *ctx) {
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "js_context");
  JSContext *self = static_cast<JSContext *>(duk_get_pointer(ctx, -1));
  duk_pop_2(ctx);

  int handle = duk_to_int(ctx, 0);
  const char *attr = duk_to_string(ctx, 1);

  Element *el = self->get_element(handle);
  if (el && el->attributes().count(attr)) {
    duk_push_string(ctx, el->attributes().at(attr).c_str());
  } else {
    duk_push_null(ctx);
  }
  return 1;
}

duk_ret_t JSContext::native_innerHTML_set(duk_context *ctx) {
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "js_context");
  JSContext *self = static_cast<JSContext *>(duk_get_pointer(ctx, -1));
  duk_pop_2(ctx);

  int handle = duk_to_int(ctx, 0);
  const char *html = duk_to_string(ctx, 1);

  Element *el = self->get_element(handle);
  if (el) {
    // el->set_inner_html(html);
    // self->tab_->rebuild_layout();
  }
  return 0;
}

duk_ret_t JSContext::native_XMLHttpRequest_send(duk_context *ctx) {
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "js_context");
  JSContext *self = static_cast<JSContext *>(duk_get_pointer(ctx, -1));
  duk_pop_2(ctx);

  if (!self || !self->tab_)
    return 0;

  const char *method = duk_to_string(ctx, 0);
  const char *url_str = duk_to_string(ctx, 1);
  const char *body = duk_to_string(ctx, 2);

  Url target_url = self->tab_->url().resolve(url_str);
  std::string page_origin = self->tab_->url().origin();
  std::string target_origin = target_url.origin();

  // Phase 3: Same-Origin Policy (SOP) check
  if (page_origin != target_origin) {
    std::cout << "[SOP] Blocked cross-origin request from " << page_origin
              << " to " << target_origin << std::endl;
    duk_push_string(ctx, "");
    return 1;
  }

  std::cout << "[JS XHR] Sending " << method << " request to "
            << target_url.href() << std::endl;
  auto response = self->tab_->http()->request(target_url, body);

  duk_push_string(ctx, response.body.c_str());
  return 1;
}

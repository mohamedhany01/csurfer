#include "JSContext.h"
#include "browser/Tab.h"
#include "css/CSSParser.h"
#include "css/CSSSelector.h"
#include "lexer/Element.h"
#include <iostream>

static void find_elements(Element *root, const CSSSelector &selector,
                          std::vector<Element *> &matches) {
  if (selector.matches(root)) {
    matches.push_back(root);
  }
  for (auto &child_lexeme : root->children()) {
    if (child_lexeme->type() == LexemeType::Element) {
      find_elements(static_cast<Element *>(child_lexeme.get()), selector,
                    matches);
    }
  }
}

JSContext::JSContext(Tab *tab) : tab_(tab) {
  ctx_ = duk_create_heap_default();
  if (!ctx_) {
    std::cerr << "Failed to create Duktape heap" << std::endl;
  }

  // Store 'this' in global stash for static callbacks
  duk_push_global_stash(ctx_);
  duk_push_pointer(ctx_, this);
  duk_put_prop_string(ctx_, -2, "js_context");
  duk_pop(ctx_);

  // Register 'log' function
  duk_push_c_function(ctx_, native_print, 1 /* nargs */);
  duk_put_global_string(ctx_, "log");

  // Register 'querySelectorAll' function
  duk_push_c_function(ctx_, native_querySelectorAll, 1 /* nargs */);
  duk_put_global_string(ctx_, "querySelectorAll");

  // Register 'getAttribute' function
  duk_push_c_function(ctx_, native_getAttribute, 2 /* nargs */);
  duk_put_global_string(ctx_, "getAttribute");
}

JSContext::~JSContext() {
  if (ctx_) {
    duk_destroy_heap(ctx_);
  }
}

void JSContext::run(const std::string &script_name, const std::string &code) {
  if (!ctx_)
    return;

  if (duk_peval_string(ctx_, code.c_str()) != 0) {
    std::cerr << "Script " << script_name
              << " crashed: " << duk_safe_to_string(ctx_, -1) << std::endl;
  }
  duk_pop(ctx_);
}

duk_ret_t JSContext::native_print(duk_context *ctx) {
  // Read the first argument as a string
  const char *str = duk_to_string(ctx, 0);
  std::cout << "JS log: " << str << std::endl;
  return 0; // No return value to JS
}

duk_ret_t JSContext::native_querySelectorAll(duk_context *ctx) {
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "js_context");
  JSContext *self = static_cast<JSContext *>(duk_get_pointer(ctx, -1));
  duk_pop_2(ctx);

  if (!self || !self->tab_ || !self->tab_->root()) {
    duk_push_array(ctx);
    return 1;
  }

  const char *selector_text = duk_to_string(ctx, 0);
  auto selector = CSSParser::parse_selector(selector_text);
  if (!selector) {
    duk_push_array(ctx);
    return 1;
  }

  std::vector<Element *> matches;
  find_elements(self->tab_->root(), *selector, matches);

  duk_push_array(ctx);
  for (size_t i = 0; i < matches.size(); ++i) {
    duk_push_int(ctx, self->get_handle(matches[i]));
    duk_put_prop_index(ctx, -2, static_cast<duk_uarridx_t>(i));
  }

  return 1;
}

duk_ret_t JSContext::native_getAttribute(duk_context *ctx) {
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1, "js_context");
  JSContext *self = static_cast<JSContext *>(duk_get_pointer(ctx, -1));
  duk_pop_2(ctx);

  if (!self) {
    duk_push_string(ctx, "");
    return 1;
  }

  int handle = duk_to_int(ctx, 0);
  const char *attr_name = duk_to_string(ctx, 1);

  Element *elt = self->get_element(handle);
  if (elt) {
    const auto &attrs = elt->attributes();
    auto it = attrs.find(attr_name);
    if (it != attrs.end()) {
      duk_push_string(ctx, it->second.c_str());
    } else {
      duk_push_string(ctx, "");
    }
  } else {
    duk_push_string(ctx, "");
  }

  return 1;
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

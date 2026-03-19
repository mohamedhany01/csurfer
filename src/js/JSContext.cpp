#include "JSContext.h"
#include "lexer/Element.h"
#include <iostream>

JSContext::JSContext() {
  ctx_ = duk_create_heap_default();
  if (!ctx_) {
    std::cerr << "Failed to create Duktape heap" << std::endl;
  }

  // Register 'log' function
  duk_push_c_function(ctx_, native_print, 1 /* nargs */);
  duk_put_global_string(ctx_, "log");
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

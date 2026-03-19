#include "JSContext.h"
#include <iostream>

JSContext::JSContext() {
  ctx_ = duk_create_heap_default();
  if (!ctx_) {
    std::cerr << "Failed to create Duktape heap" << std::endl;
  }
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

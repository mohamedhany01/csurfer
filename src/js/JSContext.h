#pragma once

#include "duktape.h"
#include <string>

class JSContext {
public:
  JSContext();
  ~JSContext();

  void run(const std::string &script_name, const std::string &code);

private:
  static duk_ret_t native_print(duk_context *ctx);
  duk_context *ctx_;
};

#pragma once

#include "duktape.h"
#include <string>

class JSContext {
public:
  JSContext();
  ~JSContext();

  void run(const std::string &script_name, const std::string &code);

private:
  duk_context *ctx_;
};

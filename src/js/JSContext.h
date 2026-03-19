#pragma once

#include "duktape.h"
#include <string>
#include <unordered_map>
#include <vector>

class Element;

class JSContext {
public:
  JSContext();
  ~JSContext();

  void run(const std::string &script_name, const std::string &code);

  int get_handle(Element *elt);
  Element *get_element(int handle);

private:
  static duk_ret_t native_print(duk_context *ctx);
  duk_context *ctx_;
  std::unordered_map<Element *, int> element_to_handle_;
  std::vector<Element *> handle_to_element_;
};

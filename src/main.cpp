#include "browser/Browser.h"
#include "request/IRequest.h"
#include "url/Url.h"
#include <iostream>

int main(int argc, char *argv[]) {
  // Story: The entry point of C Surfer. It accepts an optional URL argument.
  std::string user_url_string = (argc > 1) ? argv[1] : "about:welcome";

  try {
    Url initial_url(user_url_string);
    Browser browser;
    browser.load(initial_url);
    browser.main_loop();
  } catch (const std::exception &error) {
    std::cerr << "Fatal Error: " << error.what() << std::endl;
    return 1;
  }

  return 0;
}

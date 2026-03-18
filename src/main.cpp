#include "browser/Browser.h"
#include "request/IRequest.h"
#include "url/Url.h"
#include <iostream>

int main(int argc, char *argv[]) {
  std::string user_url = (argc > 1) ? argv[1] : "about:welcome";
  Url url(user_url);

  Browser browser;
  browser.load(url);
  browser.mainLoop();
}

#include "browser/Browser.h"
#include "request/IRequest.h"
#include "url/Url.h"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: browser <url>\n";
    return 1;
  }

  const std::string user_url = argv[1];
  Url url(user_url);

  Browser browser;
  browser.load(url);
}

#include "browser/Browser.h"
#include "request/HttpRequest.h"
#include "request/IRequest.h"
#include "url/Url.h"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: browser <url>\n";
    return 1;
  }

  Url url(argv[1]);
  std::cout << url.scheme() << std::endl;
  std::cout << url.host() << std::endl;
  std::cout << url.port() << std::endl;
  std::cout << url.path() << std::endl;

  HttpRequest req;
  std::cout << req.get(url);

  Browser browser(url);
  browser.load();
}

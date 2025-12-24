#include "Browser.h"
#include "request/HttpRequest.h"
#include <iostream>

Browser::Browser(const Url &url)
    : Browser(url, std::make_shared<HttpRequest>()) {}

Browser::Browser(const Url &url, std::shared_ptr<IRequest> request)
    : url_(url), request_(std::move(request)) {}

void Browser::load() {
  std::string body = request_->get(url_);
  render(body);
}

void Browser::render(const std::string &body) {
  bool in_tag = false;
  for (char c : body) {
    if (c == '<')
      in_tag = true;
    else if (c == '>')
      in_tag = false;
    else if (!in_tag)
      std::cout << c;
  }
}

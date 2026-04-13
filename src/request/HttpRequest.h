#pragma once
#include "IRequest.h"

class CookieJar;

class HttpRequest : public IRequest {
public:
  /**
   * Implementation of HTTP GET/POST request.
   */
  HttpResponse request(const Url &url, const std::string &payload = "",
                       const Url &referrer = {}) override;

  void set_cookie_jar(CookieJar *jar) override { cookie_jar_ = jar; }
  std::string get_cookies(const Url &url) override;
  void set_cookie(const Url &url, const std::string &value) override;

private:
  CookieJar *cookie_jar_ = nullptr;
};

#pragma once
#include "IRequest.h"

class CookieJar;

/**
 * Story: Implementation of the HTTP protocol using OpenSSL and raw sockets.
 *
 * Use-case: This is the concrete engine that performs GET and POST requests.
 * It manages the HTTP request lifecycle: resolving URLs, establishing
 * connections, sending headers, and reading the response body.
 */
class HttpRequest : public IRequest {
public:
  /**
   * Story: Executes an HTTP request. Supports both GET (empty payload)
   * and POST (with payload).
   */
  HttpResponse request(const Url &url, const std::string &payload = "",
                       const Url &referrer = {}) override;

  /**
   * Story: Connects a cookie jar to manage stateful sessions.
   */
  void set_cookie_jar(CookieJar *cookie_jar) override {
    cookie_jar_ = cookie_jar;
  }

  /**
   * Story: Gets cookies for a specific URL, adhering to domain/path rules.
   */
  std::string get_cookies(const Url &url) override;

  /**
   * Story: Manually stores a cookie in the attached jar.
   */
  void set_cookie(const Url &url, const std::string &cookie_value) override;

private:
  CookieJar *cookie_jar_ = nullptr;
};

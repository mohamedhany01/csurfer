#pragma once
#include <map>
#include <string>

#include "url/Url.h"

struct HttpResponse {
  std::map<std::string, std::string> headers;
  std::string body;
};

class CookieJar;

class IRequest {
public:
  virtual ~IRequest() = default;
  /**
   * Request a page from a URL, optionally sending a POST payload.
   * referrer is used for SameSite cookie policy and Referer header.
   */
  virtual HttpResponse request(const Url &url, const std::string &payload = "",
                               const Url &referrer = {}) = 0;

  /**
   * Associate a CookieJar with this request engine.
   */
  virtual void set_cookie_jar(CookieJar *jar) = 0;

  /**
   * Get cookies formatted for JavaScript document.cookie.
   */
  virtual std::string get_cookies(const Url &url) = 0;

  /**
   * Store a cookie from JavaScript document.cookie = "...".
   */
  virtual void set_cookie(const Url &url, const std::string &value) = 0;
};

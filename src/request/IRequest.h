#pragma once
#include <map>
#include <string>

#include "url/Url.h"

/**
 * Story: Represents the response from an HTTP server.
 * Use-case: Carries the raw body content and headers (like Content-Type)
 * back to the browser for parsing and rendering.
 */
struct HttpResponse {
  std::map<std::string, std::string> headers;
  std::string body;
};

class CookieJar;

/**
 * Story: Interface for the network request engine.
 *
 * Use-case: Abstracts the complexity of socket communication, SSL/TLS,
 * and HTTP protocols. This allows the browser to fetch resources
 * without knowing the details of how the bits are moved over the wire.
 */
class IRequest {
public:
  virtual ~IRequest() = default;

  /**
   * Story: Fetches a resource from the given URL.
   * referrer: Used for SameSite cookie checks and the 'Referer' header.
   * payload: If provided, the request is sent as an HTTP POST.
   */
  virtual HttpResponse request(const Url &url, const std::string &payload = "",
                               const Url &referrer = {}) = 0;

  /**
   * Story: Attaches a cookie storage engine to this request handler.
   */
  virtual void set_cookie_jar(CookieJar *cookie_jar) = 0;

  /**
   * Story: Retrieves cookies formatted for JavaScript's document.cookie.
   */
  virtual std::string get_cookies(const Url &url) = 0;

  /**
   * Story: Stores a cookie string (e.g., from a JS assignment).
   */
  virtual void set_cookie(const Url &url, const std::string &cookie_value) = 0;
};

#pragma once
#include <map>
#include <string>

class Url;

struct HttpResponse {
  std::map<std::string, std::string> headers;
  std::string body;
};

class IRequest {
public:
  virtual ~IRequest() = default;
  /**
   * Request a page from a URL, optionally sending a POST payload.
   */
  virtual HttpResponse request(const Url &url,
                               const std::string &payload = "") = 0;
};

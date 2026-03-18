#pragma once
#include <string>

class Url;

class IRequest {
public:
  virtual ~IRequest() = default;
  /**
   * Request a page from a URL, optionally sending a POST payload.
   */
  virtual std::string request(const Url &url,
                              const std::string &payload = "") = 0;
};

#pragma once
#include "IRequest.h"

class HttpRequest : public IRequest {
public:
  /**
   * Implementation of HTTP GET/POST request.
   */
  std::string request(const Url &url, const std::string &payload = "") override;
};

#pragma once
#include "IRequest.h"

class HttpRequest : public IRequest {
public:
  /**
   * Implementation of HTTP GET/POST request.
   */
  HttpResponse request(const Url &url,
                       const std::string &payload = "") override;
};

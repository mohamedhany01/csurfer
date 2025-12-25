#pragma once
#include "IRequest.h"

class HttpRequest : public IRequest {
public:
  std::string request(const Url &url) override;
};

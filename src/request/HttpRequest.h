#pragma once
#include "IRequest.h"

class HttpRequest : public IRequest {
public:
  std::string get(const Url &url) override;
};

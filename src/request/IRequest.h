#pragma once
#include <string>

class Url;

class IRequest {
public:
  virtual ~IRequest() = default;
  virtual std::string request(const Url &url) = 0;
};

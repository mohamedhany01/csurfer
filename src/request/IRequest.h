#pragma once
#include <string>

class Url;

class IRequest {
public:
  virtual ~IRequest() = default;
  virtual std::string get(const Url &url) = 0;
};

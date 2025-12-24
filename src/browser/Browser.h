#pragma once
#include "request/IRequest.h"
#include "url/Url.h"
#include <memory>

class Browser {
public:
  explicit Browser(const Url &url);
  Browser(const Url &url, std::shared_ptr<IRequest> request);

  void load();

private:
  Url url_;
  std::shared_ptr<IRequest> request_;

  void render(const std::string &body);
};

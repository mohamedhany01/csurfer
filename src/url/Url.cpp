#include "Url.h"
#include <cassert>

Url::Url(const std::string &raw) { parse(raw); }

void Url::parse(const std::string &raw) {
  std::string url = raw;

  auto scheme_pos = url.find("://");
  assert(scheme_pos != std::string::npos);

  scheme_ = url.substr(0, scheme_pos);
  assert(scheme_ == "http" || scheme_ == "https");

  url = url.substr(scheme_pos + 3);

  if (url.find('/') == std::string::npos) {
    url += '/';
  }

  auto slash_pos = url.find('/');
  host_ = url.substr(0, slash_pos);

  if (auto colon = host_.find(':'); colon != std::string::npos) {
    port_ = host_.substr(colon + 1);
    host_ = host_.substr(0, colon);
  }

  if (port_.empty()) {
    port_ = (scheme_ == "https") ? "443" : "80";
  }

  path_ = url.substr(slash_pos);
}

const std::string &Url::scheme() const { return scheme_; }
const std::string &Url::host() const { return host_; }
const std::string &Url::path() const { return path_; }
const std::string &Url::port() const { return port_; }

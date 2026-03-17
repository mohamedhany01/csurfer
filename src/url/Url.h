#pragma once
#include <string>

class Url {
public:
  explicit Url(const std::string &raw);

  const std::string &scheme() const;
  const std::string &host() const;
  const std::string &path() const;
  const std::string &port() const;
  
  std::string href() const;

  Url resolve(const std::string &href) const;

private:
  std::string scheme_;
  std::string host_;
  std::string path_;
  std::string port_;

  void parse(const std::string &raw);
};

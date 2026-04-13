#pragma once
#include <map>
#include <string>
#include <vector>

class Url;

struct Cookie {
  std::string name;
  std::string value;
  std::string domain;
  std::string path = "/";
  std::string same_site = "lax"; // lax, strict, none
};

/**
 * Handles storage and retrieval of HTTP cookies with SameSite enforcement.
 */
class CookieJar {
public:
  CookieJar();
  /**
   * Parse a Set-Cookie header and store the cookie.
   */
  void store_cookie(const Url &url, const std::string &set_cookie_header);

  /**
   * Retrieve all matching cookies for a request, formatted as a "Cookie" header
   * value.
   * Checks SameSite=Lax restrictions based on the referrer and method.
   */
  std::string get_cookies(const Url &target_url, const Url &referrer_url,
                          const std::string &method) const;

private:
  void save_to_disk() const;
  void load_from_disk();

  std::map<std::string, std::vector<Cookie>> cookies_;
};

#pragma once
#include <map>
#include <string>
#include <vector>

class Url;

/**
 * Story: Represents a single HTTP Cookie.
 *
 * Use-case: Stores the key-value pair along with metadata like the
 * domain, path, and security policy (SameSite).
 */
struct Cookie {
  std::string name;
  std::string value;
  std::string domain;
  std::string path = "/";
  std::string same_site = "lax"; // lax, strict, none
};

/**
 * Story: A centralized container for all browser cookies.
 *
 * Use-case: Manages the lifecycle of cookies (storage, expiration,
 * persistence). It enforces the "Same-Origin Policy" and "SameSite"
 * rules to protect user sessions from Cross-Site Request Forgery (CSRF).
 */
class CookieJar {
public:
  CookieJar();

  /**
   * Story: Parses a 'Set-Cookie' header and stores it if valid.
   */
  void store_cookie(const Url &url, const std::string &set_cookie_header);

  /**
   * Story: Retrieves all cookies that should be sent with a request to
   * target_url. referrer_url: Used to determine if the request is "same-site"
   * or "cross-site". http_method: Some SameSite policies differ for GET vs
   * POST.
   */
  std::string get_cookies(const Url &target_url, const Url &referrer_url,
                          const std::string &http_method) const;

private:
  /**
   * Story: Persists the cookie store to the local filesystem.
   */
  void save_to_disk() const;

  /**
   * Story: Loads saved cookies from previous sessions.
   */
  void load_from_disk();

  std::map<std::string, std::vector<Cookie>> cookies_;
};

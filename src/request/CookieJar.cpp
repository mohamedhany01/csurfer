#include "CookieJar.h"
#include "config/Config.h"
#include "url/Url.h"
#include "utils/StringUtils.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

CookieJar::CookieJar() { load_from_disk(); }

/**
 * Story: Parses a 'Set-Cookie' header and stores it in the jar.
 * This implementation supports Domain, Path, and SameSite attributes.
 *
 * Use-case: When a server responds with a cookie, this method ensures
 * it is persisted and available for future requests to the same domain.
 */
void CookieJar::store_cookie(const Url &url,
                             const std::string &set_cookie_header) {
  std::stringstream ss(set_cookie_header);
  std::string part;
  Cookie cookie;
  cookie.domain = url.host();

  bool first = true;
  while (std::getline(ss, part, ';')) {
    part = utils::trim(part);

    auto eq = part.find('=');
    if (first) {
      if (eq != std::string::npos) {
        cookie.name = part.substr(0, eq);
        cookie.value = part.substr(eq + 1);
      }
      first = false;
    } else {
      std::string key = (eq == std::string::npos) ? part : part.substr(0, eq);
      std::string val = (eq == std::string::npos) ? "" : part.substr(eq + 1);
      key = utils::to_lower(key);

      if (key == "domain")
        cookie.domain = val;
      else if (key == "path")
        cookie.path = val;
      else if (key == "samesite")
        cookie.same_site = utils::to_lower(val);
    }
  }

  if (!cookie.name.empty()) {
    // For simplicity, overwrite if name exists for this domain
    auto &domain_cookies = cookies_[cookie.domain];
    auto it =
        std::find_if(domain_cookies.begin(), domain_cookies.end(),
                     [&](const Cookie &c) { return c.name == cookie.name; });

    if (it != domain_cookies.end()) {
      *it = cookie;
    } else {
      domain_cookies.push_back(cookie);
    }
    save_to_disk();
  }
}

/**
 * Story: Retrieves all valid cookies for a target URL.
 * Implements SameSite (Lax/Strict) security policies to prevent CSRF.
 */
std::string CookieJar::get_cookies(const Url &target_url,
                                   const Url &referrer_url,
                                   const std::string &http_method) const {
  std::string target_host = target_url.host();
  if (cookies_.count(target_host) == 0)
    return "";

  std::string cookie_header_value;
  const auto &domain_cookies = cookies_.at(target_host);

  for (const auto &cookie : domain_cookies) {
    bool is_allowed = true;

    // SameSite=Lax enforcement
    if (cookie.same_site == "lax") {
      // If cross-origin and NOT a GET request, block the cookie (CSRF
      // prevention)
      if (!referrer_url.origin().empty() &&
          referrer_url.origin() != target_url.origin()) {
        if (http_method != "GET") {
          is_allowed = false;
        }
      }
    } else if (cookie.same_site == "strict") {
      if (referrer_url.origin() != target_url.origin()) {
        is_allowed = false;
      }
    }

    if (is_allowed) {
      if (!cookie_header_value.empty())
        cookie_header_value += "; ";
      cookie_header_value += cookie.name + "=" + cookie.value;
    }
  }

  return cookie_header_value;
}

/**
 * Story: Persists the current cookie state to a local file.
 */
void CookieJar::save_to_disk() const {
  std::ofstream f(std::string(config::COOKIE_FILE_NAME));
  if (!f.is_open())
    return;

  for (const auto &[domain, domain_cookies] : cookies_) {
    for (const auto &cookie : domain_cookies) {
      f << domain << "|" << cookie.name << "|" << cookie.value << "|"
        << cookie.same_site << "|" << cookie.path << "\n";
    }
  }
}

/**
 * Story: Loads cookies from local storage on startup.
 */
void CookieJar::load_from_disk() {
  std::ifstream f(std::string(config::COOKIE_FILE_NAME));
  if (!f.is_open())
    return;

  std::string line;
  while (std::getline(f, line)) {
    std::stringstream ss(line);
    std::string domain, name, value, samesite, path;
    if (std::getline(ss, domain, '|') && std::getline(ss, name, '|') &&
        std::getline(ss, value, '|') && std::getline(ss, samesite, '|') &&
        std::getline(ss, path)) {
      Cookie cookie;
      cookie.domain = domain;
      cookie.name = name;
      cookie.value = value;
      cookie.same_site = samesite;
      cookie.path = path;
      cookies_[domain].push_back(cookie);
    }
  }
}

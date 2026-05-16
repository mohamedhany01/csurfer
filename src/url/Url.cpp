#include "Url.h"
#include <algorithm>
#include <string_view>

/**
 * Story: Constructs a Url object from a raw string.
 * This triggers the parsing logic immediately (RAII).
 */
Url::Url(const std::string &raw_url) { parse(raw_url); }

/**
 * Story: The core parsing engine for URLs.
 * It breaks a string into scheme, host, port, and path components.
 *
 * Use-case: Essential for every network request. It validates the input
 * and throws if the URL is dangerous or nonsense.
 */
void Url::parse(const std::string &raw_url) {
  std::string working_url = raw_url;

  // Handle virtual browser schemes (like about:blank)
  if (working_url.find("about:") == 0) {
    scheme_ = "about";
    host_ = "";
    port_ = "";
    path_ = working_url.substr(6); // Skip "about:"
    return;
  }

  // Find the scheme separator (://)
  size_t scheme_separator_index = working_url.find("://");
  if (scheme_separator_index == std::string::npos) {
    throw utils::UrlError("Malformed URL: Missing '://' separator.");
  }

  scheme_ = working_url.substr(0, scheme_separator_index);

  // Scalability: We can add more schemes here in the future
  if (scheme_ != "http" && scheme_ != "https") {
    throw utils::UrlError("Unsupported protocol: " + scheme_);
  }

  working_url = working_url.substr(scheme_separator_index + 3); // Skip "://"

  // Ensure there is at least a slash for the path
  if (working_url.find('/') == std::string::npos) {
    working_url += '/';
  }

  size_t first_slash_index = working_url.find('/');
  host_ = working_url.substr(0, first_slash_index);

  if (host_.empty()) {
    throw utils::UrlError("Invalid URL: Host cannot be empty for " + scheme_);
  }

  // Extract port if present (e.g. localhost:8080)
  size_t port_separator_index = host_.find(':');
  if (port_separator_index != std::string::npos) {
    port_ = host_.substr(port_separator_index + 1);
    host_ = host_.substr(0, port_separator_index);
  }

  // Default ports if not specified
  if (port_.empty()) {
    port_ = (scheme_ == "https") ? "443" : "80";
  }

  path_ = working_url.substr(first_slash_index);
}

const std::string &Url::scheme() const { return scheme_; }
const std::string &Url::host() const { return host_; }
const std::string &Url::path() const { return path_; }
const std::string &Url::port() const { return port_; }

/**
 * Story: Reconstructs the full URL string from its components.
 */
std::string Url::href() const {
  if (scheme_ == "about") {
    return "about:" + path_;
  }
  std::string result = scheme_ + "://" + host_;
  if (!port_.empty() && port_ != "80" && port_ != "443") {
    result += ":" + port_;
  }
  result += path_;
  return result;
}

/**
 * Story: Returns the 'origin' (scheme + host + port).
 * Used for Same-Origin Policy (SOP) checks.
 */
std::string Url::origin() const {
  return scheme_ + "://" + host_ + ":" + port_;
}

/**
 * Story: Resolves a relative link against this base URL.
 *
 * Use-case: When an HTML page at 'http://foo.com/bar/' contains a link
 * to 'baz.html', this computes 'http://foo.com/bar/baz.html'.
 */
Url Url::resolve(const std::string &relative_href) const {
  // If it's already an absolute URL, return it
  if (relative_href.find("://") != std::string::npos) {
    return Url(relative_href);
  }

  // Absolute path on same host
  if (!relative_href.empty() && relative_href[0] == '/') {
    return Url(scheme_ + "://" + host_ + ":" + port_ + relative_href);
  }

  // Relative path (relative to current directory)
  std::string resolved_base_path = path_;
  size_t last_slash_index = resolved_base_path.find_last_of('/');
  if (last_slash_index != std::string::npos) {
    resolved_base_path = resolved_base_path.substr(0, last_slash_index + 1);
  } else {
    resolved_base_path = "/";
  }

  return Url(scheme_ + "://" + host_ + ":" + port_ + resolved_base_path +
             relative_href);
}

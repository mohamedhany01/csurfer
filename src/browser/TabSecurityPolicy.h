#pragma once

#include "url/Url.h"
#include <map>
#include <string>
#include <vector>

/**
 * Story: Manages Content Security Policy (CSP) for a Tab.
 *
 * Use-case: Parses the Content-Security-Policy HTTP header and checks if a
 * target URL is allowed to be loaded according to the parsed directives.
 */
class TabSecurityPolicy {
public:
  void parse_csp(const std::string &header_value, const Url &current_url);
  bool is_allowed(const Url &target_url, const std::string &directive) const;
  void clear();

private:
  std::map<std::string, std::vector<std::string>> csp_directives_;
};

#include "browser/TabSecurityPolicy.h"
#include "utils/StringUtils.h"
#include <algorithm>
#include <sstream>

void TabSecurityPolicy::parse_csp(const std::string &header_value,
                                  const Url &current_url) {
  csp_directives_.clear();
  std::stringstream ss(header_value);
  std::string directive;
  while (std::getline(ss, directive, ';')) {
    directive = utils::trim(directive);
    if (directive.empty())
      continue;

    std::stringstream dss(directive);
    std::string name;
    dss >> name; // First word is directive name (e.g. script-src)

    std::vector<std::string> origins;
    std::string origin;
    while (dss >> origin) {
      if (origin == "'self'") {
        origins.push_back(current_url.origin());
      } else {
        origins.push_back(origin);
      }
    }
    csp_directives_[name] = origins;
  }
}

bool TabSecurityPolicy::is_allowed(const Url &target_url,
                                   const std::string &directive) const {
  if (csp_directives_.find(directive) == csp_directives_.end()) {
    if (csp_directives_.find("default-src") != csp_directives_.end()) {
      auto &origins = csp_directives_.at("default-src");
      if (std::find(origins.begin(), origins.end(), "*") != origins.end()) {
        return true;
      }
      return std::find(origins.begin(), origins.end(), target_url.origin()) !=
             origins.end();
    }
    return true; // No policy means allowed
  }

  const auto &origins = csp_directives_.at(directive);

  if (std::find(origins.begin(), origins.end(), "'none'") != origins.end()) {
    return false;
  }

  if (std::find(origins.begin(), origins.end(), "*") != origins.end()) {
    return true;
  }

  return std::find(origins.begin(), origins.end(), target_url.origin()) !=
         origins.end();
}

void TabSecurityPolicy::clear() { csp_directives_.clear(); }

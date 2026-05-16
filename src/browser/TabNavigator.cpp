#include "browser/TabNavigator.h"
#include "utils/Logger.h"
#include <algorithm>

TabNavigator::TabNavigator(std::shared_ptr<IRequest> network_engine)
    : network_engine_(std::move(network_engine)), url_("http://localhost/") {}

std::string TabNavigator::load(const Url &url, const std::string &payload,
                               TabSecurityPolicy &security_policy) {
  if (history_.empty() || history_.back().href() != url.href()) {
    history_.push_back(url);
  }

  Url referrer = url_;
  url_ = url;
  CS_LOG_INFO("Navigating to: {}", url_.href());

  std::string body;
  if (url_.href() == "about:welcome") {
    body = "<html><head><title>Welcome</title></head><body>"
           "<div style=\"text-align:center; padding:50px;\">"
           "<h1>Welcome in CSurfer</h1>"
           "<p>Your lightweight, SOLID-powered browser.</p>"
           "<hr><p>Visit the project on GitHub:</p>"
           "<a href=\"https://github.com/mohamedhany01/csurfer\">"
           "mohamedhany01/csurfer</a><br><br>"
           "<p>Local Test Pages:</p>"
           "<a href=\"http://localhost:8000/\">http://localhost:8000/</a>"
           "<br><br><p><i>Enjoy surfing the solid way!</i></p>"
           "</div></body></html>";
  } else {
    try {
      auto response = network_engine_->request(url_, payload, referrer);
      body = response.body;

      if (response.headers.count("content-security-policy")) {
        security_policy.parse_csp(
            response.headers.at("content-security-policy"), url_);
      } else {
        security_policy.clear();
      }
    } catch (const std::exception &error) {
      return std::string("Network Error: ") + error.what();
    }
  }

  if (body.empty()) {
    body = "<html><body><h1>Error Loading Page</h1>"
           "<p>We couldn't reach <b>" +
           url_.href() + "</b>. Check the URL and try again.</p></body></html>";
  }

  return body;
}

std::optional<Url> TabNavigator::go_back() {
  if (history_.size() > 1) {
    history_.pop_back(); // Remove current
    Url previous_url = history_.back();
    history_.pop_back(); // Remove previous so load() can push it back
    return previous_url;
  }
  return std::nullopt;
}

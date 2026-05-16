#pragma once

#include "browser/TabSecurityPolicy.h"
#include "request/IRequest.h"
#include "url/Url.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

/**
 * Story: Manages navigation history and URL loading for a Tab.
 *
 * Use-case: Handles fetching content from the network or local resources,
 * maintaining the history stack, and enforcing security policies during
 * loading.
 */
class TabNavigator {
public:
  explicit TabNavigator(std::shared_ptr<IRequest> network_engine);

  /**
   * Story: Loads a URL and returns the content body.
   * Why: Navigation logic should be separate from layout and rendering.
   * How: Performs network request, handles special schemes, and updates
   * history. What: Returns the HTML string or error message.
   */
  std::string load(const Url &url, const std::string &payload,
                   TabSecurityPolicy &security_policy);

  /**
   * Story: Navigates back in history if possible.
   * Why: To support browser back button functionality.
   * How: Pops the current URL and returns the previous one.
   * What: Returns the previous URL or std::nullopt if no history.
   */
  std::optional<Url> go_back();

  const Url &url() const { return url_; }
  std::shared_ptr<IRequest> network_engine() const { return network_engine_; }

private:
  std::shared_ptr<IRequest> network_engine_;
  Url url_;
  std::vector<Url> history_;
};

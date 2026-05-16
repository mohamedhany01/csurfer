#pragma once
#include "url/Url.h"
#include <memory>

class Element;
class IRequest;

/**
 * Story: An interface for the host environment (usually a Tab) that
 * JSContext needs to interact with.
 *
 * Use-case: This breaks the circular dependency between JSContext and Tab,
 * allowing JSContext to be tested and refactored independently.
 */
class IJSHost {
public:
  virtual ~IJSHost() = default;

  /**
   * Story: Returns the root DOM element of the current document.
   */
  virtual Element *root() const = 0;

  /**
   * Story: Returns the current URL of the document.
   */
  virtual const Url &url() const = 0;

  /**
   * Story: Returns the network engine for performing XHR requests.
   */
  virtual std::shared_ptr<IRequest> network_engine() const = 0;
};

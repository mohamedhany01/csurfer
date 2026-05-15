#pragma once
#include <stdexcept>
#include <string>

namespace utils {

/**
 * Story: Base exception class for all C Surfer runtime errors.
 */
class CSurferError : public std::runtime_error {
public:
  explicit CSurferError(const std::string &message)
      : std::runtime_error(message) {}
};

/**
 * Story: Specific error thrown when a URL is malformed or uses an
 * unsupported scheme.
 *
 * Use-case: Allows the browser to catch invalid user input in the
 * address bar and display a friendly error page instead of crashing.
 */
class UrlError : public CSurferError {
public:
  explicit UrlError(const std::string &message) : CSurferError(message) {}
};

/**
 * Story: Specific error thrown when a network request fails (e.g., DNS error).
 */
class NetworkError : public CSurferError {
public:
  explicit NetworkError(const std::string &message) : CSurferError(message) {}
};

} // namespace utils

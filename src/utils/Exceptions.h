#pragma once
#include <stdexcept>
#include <string>

namespace utils {

/**
 * Story: A base exception for all C Surfer specific errors.
 *
 * Use-case: Allows the browser to distinguish between internal logic errors
 * and expected runtime errors (like a user typing a bad URL).
 */
class BrowserError : public std::runtime_error {
public:
  explicit BrowserError(const std::string &message)
      : std::runtime_error(message) {}
};

/**
 * Story: Thrown when a URL cannot be parsed correctly.
 *
 * Use-case: Used by the Url class to reject malformed inputs like "///" or
 * unsupported protocols like "gopher://".
 */
class UrlError : public BrowserError {
public:
  explicit UrlError(const std::string &message) : BrowserError(message) {}
};

} // namespace utils

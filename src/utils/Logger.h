#pragma once
#include <chrono>
#include <format>
#include <iomanip>
#include <iostream>
#include <string>

namespace utils {

enum class LogLevel { INFO, WARNING, ERROR };

/**
 * Story: A centralized logging system for the C Surfer engine.
 *
 * Use-case: Provides a consistent way to log messages with timestamps,
 * log levels, and future support for file-based logging or thread-safe
 * console output.
 */
class Logger {
public:
  template <typename... Args>
  static void log(LogLevel level, std::string_view fmt, Args &&...args) {
    std::string message;
    try {
      message = std::vformat(fmt, std::make_format_args(args...));
    } catch (const std::exception &e) {
      message = "LOGGING ERROR: " + std::string(e.what()) +
                " | Format: " + std::string(fmt);
    }

    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;

    char time_buf[32];
    std::strftime(time_buf, sizeof(time_buf), "%H:%M:%S",
                  std::localtime(&time_t_now));

    std::ostream &os = (level == LogLevel::ERROR) ? std::cerr : std::cout;

    os << "[" << time_buf << "." << std::setfill('0') << std::setw(3)
       << ms.count() << "] ";

    switch (level) {
    case LogLevel::INFO:
      os << "[INFO] ";
      break;
    case LogLevel::WARNING:
      os << "[WARN] ";
      break;
    case LogLevel::ERROR:
      os << "[ERROR] ";
      break;
    }

    os << message << std::endl;
  }
};

} // namespace utils

#define CS_LOG_INFO(fmt, ...)                                                  \
  ::utils::Logger::log(::utils::LogLevel::INFO, fmt, ##__VA_ARGS__)
#define CS_LOG_WARN(fmt, ...)                                                  \
  ::utils::Logger::log(::utils::LogLevel::WARNING, fmt, ##__VA_ARGS__)
#define CS_LOG_ERROR(fmt, ...)                                                 \
  ::utils::Logger::log(::utils::LogLevel::ERROR, fmt, ##__VA_ARGS__)

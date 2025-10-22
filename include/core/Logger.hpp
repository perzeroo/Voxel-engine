#pragma once

namespace Core {
enum LogLevel { DEBUG, INFO, WARNING, ERROR };
class Logger {
public:
  Logger();
  Logger(Logger &&) = default;
  Logger(const Logger &) = default;
  Logger &operator=(Logger &&) = default;
  Logger &operator=(const Logger &) = default;
  ~Logger();

private:
};

Logger::Logger() {}

Logger::~Logger() {}
} // namespace Core

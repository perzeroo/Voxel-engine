#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include "spdlog/spdlog.h"
#include <memory>
namespace Core {
// enum LogLevel { DEBUG, INFO, WARNING, ERROR };
class Log {
public:
  static void Init();

  static std::shared_ptr<spdlog::logger> GetLogger() { return m_logger; }

private:
  static std::shared_ptr<spdlog::logger> m_logger;
};
} // namespace Core
template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream &operator<<(OStream &os, const glm::vec<L, T, Q> &vector) {
  return os << glm::to_string(vector);
}

template <typename OStream, glm::length_t C, glm::length_t R, typename T,
          glm::qualifier Q>
inline OStream &operator<<(OStream &os, const glm::mat<C, R, T, Q> &matrix) {
  return os << glm::to_string(matrix);
}

template <typename OStream, typename T, glm::qualifier Q>
inline OStream &operator<<(OStream &os, glm::qua<T, Q> quaternion) {
  return os << glm::to_string(quaternion);
}

#define LOG_TRACE(...) Core::Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) Core::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) Core::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) Core::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) Core::Log::GetLogger()->critical(__VA_ARGS__)

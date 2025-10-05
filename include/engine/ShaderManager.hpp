#pragma once
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glad/glad.h>
#include <string>
#include <tracy/Tracy.hpp>
#include <unordered_map>

namespace Engine::Render {
class Shader {
public:
  Shader(const char *vertexSource, const char *fragmentSource);

  Shader(Shader &&other) noexcept : m_programID(other.m_programID) {
    other.m_programID = 0;
  }

  // Move assignment
  Shader &operator=(Shader &&other) noexcept {
    if (this != &other) {
      glDeleteProgram(m_programID); // clean up current
      m_programID = other.m_programID;
      other.m_programID = 0;
    }
    return *this;
  }

  // Delete copy constructor and assignment
  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;

  ~Shader();

  void use() const { glUseProgram(m_programID); }
  void setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(m_programID, name.c_str()), 1,
                       GL_FALSE, glm::value_ptr(mat));
  }

private:
  GLuint m_programID = 0;
};
class ShaderManager {
public:
  static Engine::Render::Shader &load(const std::string &name,
                                      const char *vertexPath,
                                      const char *fragmentPath) {
    m_shaders.emplace(name, Engine::Render::Shader(vertexPath, fragmentPath));
    return m_shaders.at(name);
  }
  static Shader &get(const std::string &name) { return m_shaders.at(name); }
  static GLuint compileShader(GLenum type, const char *source);
  static GLuint createShaderProgram(const char *vertexSource,
                                    const char *fragmentSource);

private:
  static inline std::unordered_map<std::string, Engine::Render::Shader>
      m_shaders;
};
} // namespace Engine::Render

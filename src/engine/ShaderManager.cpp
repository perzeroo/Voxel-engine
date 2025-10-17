#include "engine/ShaderManager.hpp"
#include "SDL3/SDL_log.h"
#include "engine/File.hpp"

Engine::Render::Shader::Shader(const char *vertexSource,
                               const char *fragmentSource) {
  std::string vertexCode = Engine::File::loadFileToString(vertexSource);
  std::string fragmentCode = Engine::File::loadFileToString(fragmentSource);

  m_programID = Engine::Render::ShaderManager::createShaderProgram(
      vertexCode.c_str(), fragmentCode.c_str());
}
Engine::Render::Shader::~Shader() {
  SDL_Log("Deleting shader program");
  glDeleteProgram(m_programID);
}

GLuint Engine::Render::ShaderManager::compileShader(GLenum type,
                                                    const char *source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    SDL_Log("ERROR::SHADER::COMPILATION_FAILED\n%s", infoLog);
  }
  return shader;
}

GLuint
Engine::Render::ShaderManager::createShaderProgram(const char *vertexSource,
                                                   const char *fragmentSource) {
  GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
  GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  GLint success;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    SDL_Log("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

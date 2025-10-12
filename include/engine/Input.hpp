#pragma once
#include "SDL3/SDL_mouse.h"
#include "core/Application.hpp"
#include "glm/vec2.hpp"

namespace Engine {
class Input {
public:
  static Input &instance() {
    static Input input;
    return input;
  }

  void init(Core::Application *app) {
    m_app = app;
    m_lastWindowFocusState = false;
  }

  glm::vec2 getMouseDelta() {
    if (!m_app->getWindow().isFocused()) {
      m_lastWindowFocusState = false;
      return glm::vec2(0.0f, 0.0f);
    }
    if (!m_lastWindowFocusState) {
      m_lastWindowFocusState = true;
      return glm::vec2(0.0f, 0.0f);
    }
    float deltaX, deltaY;
    SDL_GetRelativeMouseState(&deltaX, &deltaY);
    return glm::vec2(deltaX, deltaY);
  }

private:
  Core::Application *m_app;
  bool m_lastWindowFocusState;
};
} // namespace Engine

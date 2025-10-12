#pragma once
#include "SDL3/SDL.h"
#include "SDL3/SDL_video.h"
#include <glad/glad.h>

namespace Core {
class Window {
public:
  Window();
  Window(Window &&) = default;
  Window(const Window &) = default;
  Window &operator=(Window &&) = default;
  Window &operator=(const Window &) = default;
  SDL_Window *operator*() { return m_window; }
  SDL_GLContext &getGLContext() { return m_context; }
  int getWidth() const { return m_width; }
  int getHeight() const { return m_height; }
  bool isFocused() const { return m_windowFocused; }
  void handleEvent(SDL_Event *event);
  void toggleMouseFocus();
  void setMouseFocus(bool focus);
  float getAspectRatio() const {
    return static_cast<float>(m_width) / static_cast<float>(m_height);
  }
  void swapBuffers() { SDL_GL_SwapWindow(m_window); }

  ~Window();

private:
  SDL_Window *m_window;
  SDL_GLContext m_context;
  int m_width, m_height;
  bool m_windowFocused;
  bool m_mouseFocus;
};
} // namespace Core

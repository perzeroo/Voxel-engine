#include "core/Window.hpp"

namespace Core {
Window::Window() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return;
  }
  SDL_Log("Initializing SDL window");
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  m_window = SDL_CreateWindow("Voxel Engine", 1280, 720,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (!m_window) {
    SDL_Log("Couldn't create window: %s", SDL_GetError());
    return;
  }

  SDL_GLContext context = SDL_GL_CreateContext(m_window);
  if (!context) {
    SDL_Log("Couldn't create OpenGL context: %s", SDL_GetError());
    SDL_DestroyWindow(m_window);
    return;
  }
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    SDL_Log("Failed to initialize GLAD");
    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(m_window);
    return;
  }

  SDL_GL_MakeCurrent(m_window, context);
  SDL_GL_SetSwapInterval(1);
  m_windowFocused = true;
  m_width = 1280;
  m_height = 720;
}

void Window::handleEvent(SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_WINDOW_RESIZED:
    m_width = event->window.data1;
    m_height = event->window.data2;
    glViewport(0, 0, m_width, m_height);
    SDL_Log("Window resized to %dx%d", m_width, m_height);
    break;
  case SDL_EVENT_WINDOW_FOCUS_GAINED:
    m_windowFocused = true;
    setMouseFocus(m_mouseFocus);
    break;
  case SDL_EVENT_WINDOW_FOCUS_LOST:
    m_windowFocused = false;
    SDL_SetWindowRelativeMouseMode(m_window, false);
    break;
  }
}

void Window::toggleMouseFocus() {
  m_mouseFocus = !m_mouseFocus;
  if (m_windowFocused) {
    SDL_SetWindowRelativeMouseMode(m_window, m_mouseFocus);
  }
}

void Window::setMouseFocus(bool focus) {
  m_mouseFocus = focus;
  if (m_windowFocused) {
    SDL_SetWindowRelativeMouseMode(m_window, m_mouseFocus);
  }
}

Window::~Window() {
  if (m_context) {
    SDL_Log("Destroying GL context");
    SDL_GL_DestroyContext(m_context);
  }
  if (m_window) {
    SDL_Log("Destroying window");
    SDL_DestroyWindow(m_window);
  }
}
} // namespace Core

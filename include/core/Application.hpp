#pragma once
#include "SDL3/SDL.h"
#include "SDL3/SDL_video.h"
#include "entt/entt.hpp"
#include <cstdint>

namespace Core {

class Application {
public:
  Application(SDL_Window* win, SDL_GLContext &context);
  ~Application();
  SDL_Window* getWindow() const { return m_window; }
  entt::registry& getRegistry() { return m_registry; }
  void start();
  void update();
  void render();
  void onWindowResize();
  bool isWindowFocused() const { return m_windowFocused; }
  void handleEvents(SDL_Event* event);
  SDL_AppResult appQuit;

private:
  SDL_Window* m_window;
  SDL_GLContext m_context;
  entt::registry m_registry;
  int m_width, m_height;
  uint64_t m_now, m_last;
  bool m_windowFocused;
  void clearScreen();
  void perspective(float fovY, float aspect, float zNear, float zFar, float* matrix);
};

}


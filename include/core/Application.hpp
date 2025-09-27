#pragma once
#include "SDL3/SDL.h"
#include "SDL3/SDL_video.h"
#include "entt/entt.hpp"

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
  SDL_AppResult appQuit;

private:
  SDL_Window* m_window;
  SDL_GLContext m_context;
  entt::registry m_registry;
};

}


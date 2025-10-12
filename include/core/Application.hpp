#pragma once
#include "SDL3/SDL.h"
#include "SDL3/SDL_video.h"
#include "core/Window.hpp"
#include "engine/chunk/ChunkManager.hpp"
#include "engine/world/WorldGenerator.hpp"
#include "entt/entt.hpp"
#include <cstdint>
#include <glad/glad.h>

namespace Core {

class Application {
public:
  Application();
  ~Application();
  Window &getWindow() { return m_window; }
  entt::registry &getRegistry() { return m_registry; }
  void update();
  void render();
  void handleEvents(SDL_Event *event);
  SDL_AppResult appQuit;

private:
  entt::registry m_registry;
  uint64_t m_now, m_last;
  std::unique_ptr<Engine::Chunk::ChunkManager> m_chunkManager;
  Window m_window;
  entt::entity m_activeCamera = entt::null;
  int m_renderDistance = 8;
  void clearScreen();
  void perspective(float fovY, float aspect, float zNear, float zFar,
                   float *matrix);
  void renderTopRightInfo(const std::string &info);
  void renderDebugSettingsWindow();
};

} // namespace Core

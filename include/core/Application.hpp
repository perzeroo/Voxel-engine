#pragma once
#include "SDL3/SDL.h"
#include "SDL3/SDL_video.h"
#include "core/Window.hpp"
#include "engine/Texture.hpp"
#include "engine/chunk/ChunkManager.hpp"
#include "engine/world/WorldGenerator.hpp"
#include "entt/entt.hpp"
#include "imgui.h"
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
  Engine::Texture::TextureManager m_textureManager;
  entt::entity m_activeCamera = entt::null;
  int m_renderDistance = 8;
  ImColor m_skyColor = ImColor(0.53f, 0.81f, 0.92f);
  void clearScreen();
  void renderTopRightInfo(const std::string &info);
  void renderDebugSettingsWindow();
};

} // namespace Core

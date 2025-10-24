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
  Engine::TextureManager &getTextureManager() { return m_textureManager; }
  entt::entity getActiveCamera() { return m_activeCamera; }
  void update();
  void render();
  void handleEvents(SDL_Event *event);
  SDL_AppResult appQuit;
  static Application &instance() { return *s_instance; }

private:
  entt::registry m_registry;
  Window m_window;

  uint64_t m_now, m_last;

  Engine::Chunk::ChunkManager m_chunkManager;

  Engine::TextureManager m_textureManager;
  entt::entity m_activeCamera = entt::null;
  int m_renderDistance = 8;
  ImColor m_skyColor = ImColor(0.53f, 0.81f, 0.92f);
  static Application *s_instance;
  void clearScreen();
  void renderTopRightInfo(const std::string &info);
  void renderDebugSettingsWindow();
};

} // namespace Core
#define gApp Core::Application::instance()

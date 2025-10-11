#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_timer.h"
#include "engine/Camera.hpp"
#include "engine/Input.hpp"
#include "engine/ShaderManager.hpp"
#include "engine/Transform.hpp"
#include "engine/ImGuiHelper.hpp"
#include "engine/chunk/ChunkManager.hpp"
#include "engine/chunk/ChunkMesh.hpp"
#include "engine/chunk/ChunkPosition.hpp"
#include "engine/utils/Utils.hpp"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include <core/Application.hpp>
#include <memory>
#include <tracy/Tracy.hpp>


namespace Core {
Application::Application(SDL_Window *win, SDL_GLContext &context)
    : m_chunkManager(
          std::make_unique<Engine::Chunk::ChunkManager>(m_registry)) {
  m_window = win;
  m_context = context;
  appQuit = SDL_APP_CONTINUE;

  glEnable(GL_DEPTH_TEST);

  Engine::Render::ShaderManager::load("default", "assets/shaders/basic.vert",
                                      "assets/shaders/basic.frag");
  Engine::Render::ShaderManager::load("chunk", "assets/shaders/chunk.vert",
                                      "assets/shaders/chunk.frag");
  onWindowResize();

  SDL_SetWindowRelativeMouseMode(m_window, true);
  m_windowFocused = true;
  Engine::Input::instance().init(this);

  m_activeCamera = Engine::Utils::cameraWithControllerEntity(m_registry);

  Engine::ImGuiHelper::init(m_window, m_context);

  m_now = SDL_GetPerformanceCounter();
}

Application::~Application() {
  auto view = m_registry.view<Engine::Chunk::ChunkMesh>();
  for (auto entity : view) {
    m_registry.remove<Engine::Chunk::ChunkMesh>(entity);
  }
  Engine::ImGuiHelper::shutdown();
}


void Application::update() {
  ZoneScopedN("Application::update");
  m_last = m_now;
  m_now = SDL_GetPerformanceCounter();
  float dt =
      static_cast<double>(m_now - m_last) / SDL_GetPerformanceFrequency();

  Engine::CameraControllerSystem::update(m_registry, dt);

  m_chunkManager->processChunkUpdates();
  m_chunkManager->buildChunkMeshes();

  if (m_activeCamera == entt::null) {
    return;
  }

  auto playerChunkPos =
      m_registry.get<Engine::Transform>(m_activeCamera).getChunkPosition();

  auto renderDistance = 4;

  m_chunkManager->deleteOldChunks(playerChunkPos.x, playerChunkPos.z,
                                  renderDistance);

  m_chunkManager->loadNewChunks(playerChunkPos, renderDistance);
}

void Application::handleEvents(SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_WINDOW_FOCUS_GAINED:
    m_windowFocused = true;
    SDL_SetWindowRelativeMouseMode(m_window, true);
    break;
  case SDL_EVENT_WINDOW_FOCUS_LOST:
    m_windowFocused = false;
    SDL_SetWindowRelativeMouseMode(m_window, false);
    break;
  case SDL_EVENT_KEY_DOWN:
    if (event->key.key == SDLK_N) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else if (event->key.key == SDLK_M) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else if (event->key.key == SDLK_ESCAPE) {
        m_windowFocused = !m_windowFocused;
      SDL_SetWindowRelativeMouseMode(m_window, m_windowFocused);
    }
    break;
  default:
    break;
  }
  ImGui_ImplSDL3_ProcessEvent(event);
}

void Application::render() {
  {
    ZoneScopedN("Application::render");
    Engine::ImGuiHelper::beginFrame();
    clearScreen();
    glm::mat4 viewProjection = Engine::CameraSystem::update(
        m_registry, (float)m_width / (float)m_height);

    // GLint vpLoc = glGetUniformLocation(m_shaderProgram, "u_ViewProjection");
    // glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(viewProjection));

    auto renderedChunksView =
        m_registry
            .view<Engine::Chunk::ChunkMesh, Engine::Chunk::ChunkMeshRenderer,
                  Engine::Chunk::ChunkPosition>();

    const Engine::Render::Shader &chunkShader =
        Engine::Render::ShaderManager::get("chunk");
    chunkShader.use();
    chunkShader.setMat4("u_ViewProjection", viewProjection);
    
    unsigned int totalTriangles = 0;

    for (auto entity : renderedChunksView) {
      auto &mesh = renderedChunksView.get<Engine::Chunk::ChunkMesh>(entity);
      totalTriangles += static_cast<unsigned int>(mesh.indices.size()) / 3;
      auto &renderer =
          renderedChunksView.get<Engine::Chunk::ChunkMeshRenderer>(entity);
      auto &position =
          renderedChunksView.get<Engine::Chunk::ChunkPosition>(entity);
      renderer.render(mesh, position, viewProjection);
    }
    ImGuiIO &io = ImGui::GetIO();
    std::string sTotalTriangles = "Triangles: " + std::to_string(totalTriangles);
    ImVec2 size = ImGui::CalcTextSize(sTotalTriangles.c_str());
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - size.x - 20, 10));
    ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
    ImGui::Begin("Stats", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs);
    ImGui::Text("%s", sTotalTriangles.c_str());
    ImGui::End();
    ImGui::ShowDemoWindow();
    Engine::ImGuiHelper::endFrame();
  }
  
  SDL_GL_SwapWindow(m_window);
}

void Application::onWindowResize() {
  int w, h;
  SDL_GetWindowSize(m_window, &w, &h);
  m_width = w;
  m_height = h;
}

void Application::clearScreen() {
  glViewport(0, 0, m_width, m_height);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
} // namespace Core

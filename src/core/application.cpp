#include "core/Common.hpp"
#include "core/Window.hpp"
#include "engine/Engine.hpp"
#include "glm/geometric.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_timer.h"
#include "engine/Camera.hpp"
#include "engine/ImGuiHelper.hpp"
#include "engine/Input.hpp"
#include "engine/ShaderManager.hpp"
#include "engine/Transform.hpp"
#include "engine/chunk/ChunkManager.hpp"
#include "engine/chunk/ChunkMesh.hpp"
#include "engine/chunk/ChunkPosition.hpp"
#include "engine/utils/Utils.hpp"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "thirdparty/stb/stb_image.hpp"
#include <core/Application.hpp>
#include <memory>
#include <tracy/Tracy.hpp>

namespace Core {
Application::Application()
    : m_chunkManager(
          std::make_unique<Engine::Chunk::ChunkManager>(m_registry)) {
  SDL_SetAppMetadata("Voxel Engine", "1.0", "me.perzero.voxel-engine");

  if (*m_window == nullptr) {
    SDL_Log("Couldn't create window");
    appQuit = SDL_APP_FAILURE;
    return;
  }
  appQuit = SDL_APP_CONTINUE;

  glEnable(GL_DEPTH_TEST);

  Engine::Render::ShaderManager::load("default", "assets/shaders/basic.vert",
                                      "assets/shaders/basic.frag");
  Engine::Render::ShaderManager::load("chunk", "assets/shaders/chunk.vert",
                                      "assets/shaders/chunk.frag");
  m_window.setMouseFocus(true);

  Engine::Input::instance().init(this);

  m_activeCamera = Engine::Utils::cameraWithControllerEntity(m_registry);

  Engine::ImGuiHelper::init(*m_window, m_window.getGLContext());

  int width, height, channels;
  unsigned char *imageData =
      stbi_load("assets/textures/VoxelSprites.png", &width, &height, &channels,
                4); // 4 = force RGBA

  if (!imageData) {
    appQuit = SDL_APP_FAILURE;
    return;
  }

  glGenTextures(1, &m_voxelTextureID);
  glBindTexture(GL_TEXTURE_2D, m_voxelTextureID);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, imageData);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(imageData);
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
  if (m_window.isFocused()) {
    Engine::CameraControllerSystem::update(m_registry, dt);
  }

  if (m_activeCamera == entt::null) {
    return;
  }

  auto playerChunkPos =
      m_registry.get<Engine::Transform>(m_activeCamera).getChunkPosition();

  m_chunkManager->processChunkUpdates();
  m_chunkManager->buildChunkMeshes();

  auto chunksDeleted = m_chunkManager->deleteOldChunks(
      playerChunkPos.x, playerChunkPos.z, m_renderDistance);
  if (chunksDeleted)
    m_chunkManager->loadNewChunks(playerChunkPos, m_renderDistance);
}

void Application::handleEvents(SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_QUIT:
    appQuit = SDL_APP_SUCCESS;
    break;
  case SDL_EVENT_KEY_DOWN:
    if (event->key.key == SDLK_N) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else if (event->key.key == SDLK_M) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else if (event->key.key == SDLK_ESCAPE) {
      m_window.toggleMouseFocus();
    }
    break;
  default:
    break;
  }
  m_window.handleEvent(event);
  ImGui_ImplSDL3_ProcessEvent(event);
}

void Application::render() {
  {
    ZoneScopedN("Application::render");
    Engine::ImGuiHelper::beginFrame();
    clearScreen();
    glm::mat4 viewProjection =
        Engine::CameraSystem::update(m_registry, m_window.getAspectRatio());

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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_voxelTextureID);

    unsigned int totalTriangles = 0;
    unsigned int totalChunks = 0;
    auto &camera = m_registry.get<Engine::Camera>(m_activeCamera);
    auto &cameraTransform = m_registry.get<Engine::Transform>(m_activeCamera);
    for (auto entity : renderedChunksView) {
      auto &mesh = renderedChunksView.get<Engine::Chunk::ChunkMesh>(entity);
      if (!m_registry.all_of<Engine::Dirty>(entity) &&
          !m_registry.all_of<Engine::InUse>(entity)) {
        mesh.clean();
      }
      auto &position =
          renderedChunksView.get<Engine::Chunk::ChunkPosition>(entity);
      auto fPos = position.toWorldPosition() - cameraTransform.position +
                  (camera.front * static_cast<float>(CHUNK_WIDTH) / 2.0f);
      fPos += glm::vec3(static_cast<float>(CHUNK_WIDTH) / 2.0f);
      auto fPosNorm = glm::normalize(fPos);
      float dot =
          glm::dot(camera.front, fPosNorm); // camera.front is normalized
      // float threshold = cos(glm::radians(60.0f) / 2.0f);
      float threshold = 0.0f;
      if (dot < threshold) {
        continue;
      }

      totalTriangles += static_cast<unsigned int>(mesh.indicesSize);
      totalChunks++;
      auto &renderer =
          renderedChunksView.get<Engine::Chunk::ChunkMeshRenderer>(entity);
      renderer.render(mesh, position, viewProjection);
    }
    totalTriangles /= 3;
    std::string sTotalTriangles =
        "Triangles: " + std::to_string(totalTriangles);
    std::string sTotalChunks = "Chunks: " + std::to_string(totalChunks);
    std::string positionString =
        "Position: " +
        std::to_string(
            m_registry.get<Engine::Transform>(m_activeCamera).position.x) +
        ", " +
        std::to_string(
            m_registry.get<Engine::Transform>(m_activeCamera).position.y) +
        ", " +
        std::to_string(
            m_registry.get<Engine::Transform>(m_activeCamera).position.z);
    std::string statsString =
        sTotalTriangles + " | " + sTotalChunks + "\n" + positionString;
    renderTopRightInfo(statsString);
    renderDebugSettingsWindow();
    Engine::ImGuiHelper::endFrame();
  }

  m_window.swapBuffers();
}

void Application::renderDebugSettingsWindow() {
  auto &cc = m_registry.get<Engine::CameraController>(m_activeCamera);
  ImGui::Begin("Settings");
  ImGui::Text("Render Distance");
  if (ImGui::SliderInt("##RenderDistance", &m_renderDistance, 1, 32)) {
    // m_chunkManager->forceChunkReload();
  }
  if (ImGui::SliderFloat("Movement Speed", &cc.movementSpeed, 1.0f, 50.0f)) {
  }
  if (ImGui::ColorEdit3("Sky Color", (float *)&m_skyColor)) {
    // Update sky color
  }
  ImGui::End();
}

void Application::renderTopRightInfo(const std::string &info) {
  ImGuiIO &io = ImGui::GetIO();
  ImVec2 size = ImGui::CalcTextSize(info.c_str());
  ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - size.x - 25, 10));
  ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
  ImGui::Begin(
      "Info", nullptr,
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
          ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
          ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs);
  ImGui::Text("%s", info.c_str());
  ImGui::End();
}

void Application::clearScreen() {
  glViewport(0, 0, m_window.getWidth(), m_window.getHeight());
  ImColor skyColor = m_skyColor;
  glClearColor(skyColor.Value.x, skyColor.Value.y, skyColor.Value.z, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
} // namespace Core

#include "engine/chunk/ChunkData.hpp"
#include "core/Common.hpp"
#include <core/Application.hpp>

namespace Core {
Application::Application(SDL_Window* win, SDL_GLContext &context)  {
  m_window = win;
  m_context = context;
  appQuit = SDL_APP_CONTINUE;
}

void Application::start() {
  Engine::Chunk::ChunkData chunk;
  for (int i = 0; i < CHUNK_SIZE; i++) {
    chunk.voxels[i].type = 1;
  }
  const auto entity = m_registry.create();
  m_registry.emplace<Engine::Chunk::ChunkData>(entity, chunk);
}

void Application::update() {

}

Application::~Application() {
}
  
}

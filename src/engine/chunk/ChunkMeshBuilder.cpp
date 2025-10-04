#include "SDL3/SDL_log.h"
#include "SDL3/SDL_timer.h"
#include "core/Common.hpp"
#include "engine/Engine.hpp"
#include "engine/Render.hpp"
#include "engine/chunk/ChunkData.hpp"
#include <engine/chunk/ChunkMeshBuilder.hpp>
#include <memory>

void Engine::Chunk::ChunkMeshBuilder::on_construct(entt::registry &registry,
                                                   entt::entity entity) {
  // Engine::Chunk::ChunkMeshBuilder::on_chunk_data_change(registry, entity);
  // registry.emplace<Engine::Dirty>(entity);
}

void Engine::Chunk::ChunkMeshBuilder::on_chunk_data_change(
    entt::registry &registry, entt::entity entity) {
  auto &chunkData = registry.get<ChunkData>(entity);
  if (!registry.all_of<ChunkMesh>(entity)) {
    registry.emplace<ChunkMesh>(entity);
  }
  auto &chunkMesh = registry.get<ChunkMesh>(entity);
  // build(chunkData, chunkMesh);
}

#include <engine/chunk/ChunkData.hpp>
#include <engine/chunk/ChunkMeshBuilder.hpp>

void Engine::Chunk::ChunkData::on_update(entt::registry &registry, entt::entity entity) {
  if (registry.all_of<ChunkMeshBuilder>(entity)) {
    ChunkMeshBuilder::on_chunk_data_change(registry, entity);
  }
}

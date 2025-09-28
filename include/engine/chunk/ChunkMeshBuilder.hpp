#pragma once

#include "ChunkData.hpp"
#include "ChunkMesh.hpp"
#include "entt/entt.hpp"

namespace Engine::Chunk {
struct ChunkMeshBuilder {
  static void on_construct(entt::registry& registry, entt::entity entity);
  static void on_chunk_data_change(entt::registry& registry, entt::entity entity);
  static void build(ChunkData &chunkData, ChunkMesh &chunkMesh); 
};
}

#pragma once

#include "ChunkMesh.hpp"
#include "entt/entt.hpp"

namespace Engine::Chunk {
struct ChunkMeshBuilder {
  static void on_construct(entt::registry& registry, entt::entity entity);
  static void on_chunk_data_change(entt::registry& registry, entt::entity entity);
  static void build(entt::registry& registry, entt::entity entity, std::unordered_map<entt::entity, ChunkMesh>& meshMap); 
};
}

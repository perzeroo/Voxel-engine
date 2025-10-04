#pragma once
#include <entt/entt.hpp>
#include "core/Common.hpp"
#include "engine/voxel/Voxel.hpp"
#include "engine/chunk/ChunkMesh.hpp"
#include <array>
namespace Engine::Chunk {
struct ChunkData {
  static void on_update(entt::registry& registry, entt::entity entity);
  std::array<Engine::Voxel::Voxel, CHUNK_SIZE> voxels;
  std::shared_ptr<Engine::Chunk::ChunkMesh> buildChunkMesh();
};

inline int getIdx(int x, int y, int z) {
  return x + y * CHUNK_WIDTH + z * CHUNK_WIDTH * CHUNK_WIDTH;
}
}

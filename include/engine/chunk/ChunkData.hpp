#pragma once
#include "core/Common.hpp"
#include "engine/chunk/ChunkMesh.hpp"
#include "engine/voxel/Voxel.hpp"
#include <array>
#include <entt/entt.hpp>
namespace Engine::Chunk {

inline int getIdx(int x, int y, int z) {
  return x + y * CHUNK_WIDTH + z * CHUNK_WIDTH * CHUNK_WIDTH;
}

struct ChunkData;

struct ChunkNeighborhood {
  const ChunkData *px = nullptr;
  const ChunkData *nx = nullptr;
  const ChunkData *py = nullptr;
  const ChunkData *ny = nullptr;
  const ChunkData *pz = nullptr;
  const ChunkData *nz = nullptr;
};

struct ChunkData {
  static void on_update(entt::registry &registry, entt::entity entity);
  std::array<Engine::Voxel::Voxel, CHUNK_SIZE> voxels;
  std::shared_ptr<Engine::Chunk::ChunkMesh>
  buildChunkMesh(const Engine::Chunk::ChunkNeighborhood &neighborhood);
  uint16_t getType(int x, int y, int z) const {
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_WIDTH || z < 0 ||
        z >= CHUNK_WIDTH) {
      return 1;
    }
    return voxels[getIdx(x, y, z)].type;
  }
};

} // namespace Engine::Chunk

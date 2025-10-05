#pragma once
#include "core/Common.hpp"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include <array>
#include <functional>

namespace Engine::Chunk {
struct ChunkPosition {
  int x = 0;
  int y = 0;
  int z = 0;
  glm::vec3 toWorldPosition() const {
    return glm::vec3(x * CHUNK_WIDTH, y * CHUNK_WIDTH, z * CHUNK_WIDTH);
  }
  std::array<ChunkPosition, 6> neighbors() const {
    return {ChunkPosition{x + 1, y, z}, ChunkPosition{x - 1, y, z},
            ChunkPosition{x, y + 1, z}, ChunkPosition{x, y - 1, z},
            ChunkPosition{x, y, z + 1}, ChunkPosition{x, y, z - 1}};
  }
  bool operator==(const ChunkPosition &other) const {
    return x == other.x && y == other.y && z == other.z;
  }
};
} // namespace Engine::Chunk

namespace std {
template <> struct hash<Engine::Chunk::ChunkPosition> {
  std::size_t operator()(const Engine::Chunk::ChunkPosition &k) const {
    std::size_t seed = 0;
    seed ^= std::hash<int>()(k.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<int>()(k.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<int>()(k.z) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
  }
};
} // namespace std

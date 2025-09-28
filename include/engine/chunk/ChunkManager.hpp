#pragma once
#include "engine/chunk/ChunkData.hpp"
#include "engine/chunk/ChunkPosition.hpp"
#include "engine/voxel/Voxel.hpp"
#include "engine/world/WorldGenerator.hpp"
#include <entt/entt.hpp>
#include <unordered_map>

namespace Engine::Chunk {
class ChunkManager {
public:
  ChunkManager(entt::registry &registry)
      : m_registry(registry), m_worldGenerator() {}

  // ChunkManager(ChunkManager &&) = default;
  // ChunkManager(const ChunkManager &) = default;
  // ChunkManager &operator=(ChunkManager &&) = delete;
  // ChunkManager &operator=(const ChunkManager &) = delete;

  entt::entity createChunk(int x, int y, int z);
  entt::entity getChunk(const Engine::Chunk::ChunkPosition &position) const {
    auto it = m_chunkMap.find(position);
    if (it != m_chunkMap.end()) {
      return it->second;
    }
    return entt::null;
  }

  Engine::Voxel::Voxel getBlock(int x, int y, int z) const {
    Engine::Chunk::ChunkPosition chunkPos = {x / 16, y / 16, z / 16};
    entt::entity chunkEntity = getChunk(chunkPos);
    if (chunkEntity == entt::null) {
      return {0};
    }
    auto &chunkData = m_registry.get<Engine::Chunk::ChunkData>(chunkEntity);
    int localX = x % 16;
    int localY = y % 16;
    int localZ = z % 16;
    int index = Engine::Chunk::getIdx(localX, localY, localZ);
    if (index < 0 || index >= 4096) {
      return {0};
    }
    return chunkData.voxels[index];
  }

  void deleteOldChunks(int playerX, int playerZ, int radius);
  void deleteChunk(int x, int y, int z);

  ~ChunkManager() = default;
  static int CHUNKS_GENERATED_THIS_FRAME;

private:
  entt::registry &m_registry;
  Engine::World::WorldGenerator m_worldGenerator;
  std::unordered_map<Engine::Chunk::ChunkPosition, entt::entity> m_chunkMap;
};
// class MyClass {
// public:
//   MyClass();
//   MyClass(MyClass &&) = default;
//   MyClass(const MyClass &) = default;
//   MyClass &operator=(MyClass &&) = default;
//   MyClass &operator=(const MyClass &) = default;
//   ~MyClass();
//
// private:
// };
//
// MyClass::MyClass() {}
//
// MyClass::~MyClass() {}
} // namespace Engine::Chunk

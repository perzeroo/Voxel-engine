#pragma once
#include "engine/chunk/ChunkData.hpp"
#include "engine/chunk/ChunkPosition.hpp"
#include "engine/chunk/ChunkUpdate.hpp"
#include "engine/voxel/Voxel.hpp"
#include "engine/world/WorldGenerator.hpp"
#include "thirdparty/cameron314/concurrentqueue.hpp"
#include <entt/entt.hpp>
#include <unordered_map>

namespace Engine::Chunk {
class ChunkManager {
public:
  ChunkManager(entt::registry &registry)
      : m_registry(registry), m_worldGenerator() {}

  entt::entity createChunk(int x, int y, int z);
  entt::entity getChunk(const Engine::Chunk::ChunkPosition &position) const {
    auto it = m_chunkMap.find(position);
    if (it != m_chunkMap.end() && m_registry.valid(it->second)) {
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
    int localX = x % 32;
    int localY = y % 32;
    int localZ = z % 32;
    int index = Engine::Chunk::getIdx(localX, localY, localZ);
    if (index < 0 || index >= CHUNK_SIZE) {
      return {0};
    }
    return chunkData.voxels[index];
  }

  void generateChunkData(entt::entity chunkEntity);
  void buildChunkMeshes();

  void deleteOldChunks(int playerX, int playerZ, int radius);
  void deleteChunk(int x, int y, int z);
  void buildChunkMesh(entt::entity chunkEntity,ChunkMesh& chunkMesh,
                      const ChunkNeighborhood &neighborhood);
  void processChunkUpdates();
  void loadNewChunks(ChunkPosition playerPos, int renderDistance);
  void tryAddChunkToBuildQueue(ChunkPosition pos);

  ~ChunkManager() = default;

private:
  entt::registry &m_registry;
  Engine::World::WorldGenerator m_worldGenerator;
  std::unordered_map<Engine::Chunk::ChunkPosition, entt::entity> m_chunkMap;
  moodycamel::ConcurrentQueue<std::shared_ptr<Engine::Chunk::ChunkUpdate>>
      m_chunkUpdateQueue;
  unsigned int m_maxChunkUpdatesPerFrame = 8;
  unsigned int m_chunksUpdatedThisFrame = 0;
};
} // namespace Engine::Chunk

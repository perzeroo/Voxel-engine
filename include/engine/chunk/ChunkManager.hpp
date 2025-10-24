#pragma once
#include "engine/Texture.hpp"
#include "engine/chunk/ChunkData.hpp"
#include "engine/chunk/ChunkMesh.hpp"
#include "engine/chunk/ChunkPosition.hpp"
#include "engine/chunk/ChunkUpdate.hpp"
#include "engine/voxel/Voxel.hpp"
#include "engine/world/WorldGenerator.hpp"
#include "thirdparty/cameron314/concurrentqueue.hpp"
#include <entt/entt.hpp>
#include <memory>
#include <unordered_map>

namespace Engine::Chunk {
class ChunkManager {
public:
  ChunkManager() = default;

  entt::entity createChunk(int x, int y, int z);
  entt::entity getChunk(const Engine::Chunk::ChunkPosition &position) const;

  Engine::Voxel::Voxel getBlock(int x, int y, int z) const;
  void generateChunkData(entt::entity chunkEntity);
  void buildChunkMeshes();

  bool deleteOldChunks(int playerX, int playerZ, int radius);
  void deleteChunk(int x, int y, int z);
  void buildChunkMesh(entt::entity chunkEntity,
                      std::shared_ptr<ChunkMeshData> chunkMesh,
                      const ChunkNeighborhood &neighborhood);
  void processChunkUpdates();
  void loadNewChunks(ChunkPosition playerPos, int renderDistance);
  void tryAddChunkToBuildQueue(ChunkPosition pos);
  void setRenderDistance(unsigned int distance) { m_renderDistance = distance; }
  unsigned int getRenderDistance() const { return m_renderDistance; }
  void update();
  void render(const glm::mat4 &viewProjection);

  void init();

  ~ChunkManager() = default;

private:
  Engine::World::WorldGenerator m_worldGenerator;
  std::unordered_map<Engine::Chunk::ChunkPosition, entt::entity> m_chunkMap;
  moodycamel::ConcurrentQueue<std::shared_ptr<Engine::Chunk::ChunkUpdate>>
      m_chunkUpdateQueue;
  moodycamel::ConcurrentQueue<entt::entity> m_chunkSetupQueue;
  unsigned int m_maxChunkSetupsPerFrame = 16;
  unsigned int m_chunksSetupThisFrame = 0;
  unsigned int m_renderDistance = 8;
  std::shared_ptr<Engine::Texture> m_voxelTexture;
};
} // namespace Engine::Chunk

#include "engine/chunk/ChunkManager.hpp"
#include "SDL3/SDL_log.h"
#include "engine/chunk/ChunkData.hpp"
#include "engine/chunk/ChunkMeshBuilder.hpp"
#include "engine/chunk/ChunkPosition.hpp"
#include "engine/world/WorldGenerator.hpp"

int Engine::Chunk::ChunkManager::CHUNKS_GENERATED_THIS_FRAME = 0;
entt::entity Engine::Chunk::ChunkManager::createChunk(int x, int y, int z) {
  auto chunkPos = Engine::Chunk::ChunkPosition{x, y, z};
  if (getChunk(chunkPos) != entt::null) {
    // SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
    //             "Chunk at (%d, %d, %d) already exists", x, y, z);
    return entt::null; // Chunk already exists
  }

  entt::entity chunkEntity = m_registry.create();
  Engine::Chunk::ChunkData chunkData;
  m_worldGenerator.generateChunk(x, y, z, chunkData);
  m_registry.emplace<Engine::Chunk::ChunkData>(chunkEntity, chunkData);
  m_registry.emplace<Engine::Chunk::ChunkMeshBuilder>(chunkEntity);
  m_registry.emplace<Engine::Chunk::ChunkMeshRenderer>(
      chunkEntity, Engine::Render::ShaderManager::get("chunk"));
  m_registry.emplace<Engine::Chunk::ChunkPosition>(chunkEntity, chunkPos);

  m_chunkMap[chunkPos] = chunkEntity;
  return chunkEntity;
}

void Engine::Chunk::ChunkManager::deleteOldChunks(int playerX, int playerZ,
                                                  int radius) {
  std::vector<Engine::Chunk::ChunkPosition> chunksToDelete;
  for (const auto &pair : m_chunkMap) {
    const auto &chunkPos = pair.first;
    int dx = chunkPos.x - playerX;
    int dz = chunkPos.z - playerZ;
    if (dx * dx + dz * dz > radius * radius) {
      chunksToDelete.push_back(chunkPos);
    }
  }

  for (const auto &chunkPos : chunksToDelete) {
    deleteChunk(chunkPos.x, chunkPos.y, chunkPos.z);
  }
}

void Engine::Chunk::ChunkManager::deleteChunk(int x, int y, int z) {
  auto chunkPos = Engine::Chunk::ChunkPosition{x, y, z};
  entt::entity chunkEntity = getChunk(chunkPos);
  if (chunkEntity == entt::null) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Chunk at (%d, %d, %d) does not exist", x, y, z);
    return; // Chunk does not exist
  }

  m_registry.destroy(chunkEntity);
  m_chunkMap.erase(chunkPos);
}

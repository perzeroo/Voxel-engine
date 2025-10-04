#include "engine/chunk/ChunkManager.hpp"
#include "SDL3/SDL_log.h"
#include "engine/ThreadPool.hpp"
#include "engine/chunk/ChunkData.hpp"
#include "engine/chunk/ChunkMesh.hpp"
#include "engine/chunk/ChunkMeshBuilder.hpp"
#include "engine/chunk/ChunkPosition.hpp"
#include "engine/world/WorldGenerator.hpp"
#include "engine/Engine.hpp"

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
  m_registry.emplace<Engine::Dirty>(chunkEntity);

  ThreadPool::getInstance().enqueueTask([this, chunkEntity]() {
    addChunkToQueue(chunkEntity);
  });

  m_chunkMap[chunkPos] = chunkEntity;
  return chunkEntity;
}

void Engine::Chunk::ChunkManager::addChunkToQueue(entt::entity chunkEntity) {
  if (!m_registry.all_of<Engine::Chunk::ChunkData>(chunkEntity)) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Entity is not a chunk data entity");
    return;
  }
  auto &chunkData = m_registry.get<Engine::Chunk::ChunkData>(chunkEntity);
  auto chunkUpdate = chunkData.buildChunkMesh();
  m_chunkUpdateQueue.enqueue(
      std::make_shared<Engine::Chunk::ChunkUpdate>(chunkEntity, chunkUpdate));
}

void Engine::Chunk::ChunkManager::processChunkUpdates() {
  std::shared_ptr<Engine::Chunk::ChunkUpdate> chunkUpdate;
  while (m_chunkUpdateQueue.try_dequeue(chunkUpdate)) {
    if (!m_registry.valid(chunkUpdate->entity)) {
      continue; // Entity no longer exists
    }
    if (!m_registry.all_of<Engine::Chunk::ChunkMeshRenderer>(
            chunkUpdate->entity)) {
      continue; // Entity is not a chunk mesh renderer entity
    }
    m_registry.replace<Engine::Chunk::ChunkMesh>(
        chunkUpdate->entity, *(chunkUpdate->newMesh));
    m_registry.remove<Engine::Dirty>(chunkUpdate->entity);
  }
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

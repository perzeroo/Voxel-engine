#include "engine/chunk/ChunkManager.hpp"
#include "SDL3/SDL_timer.h"
#include "core/Log.hpp"
#include "engine/Engine.hpp"
#include "engine/ThreadPool.hpp"
#include "engine/chunk/ChunkData.hpp"
#include "engine/chunk/ChunkMesh.hpp"
#include "engine/chunk/ChunkPosition.hpp"
#include "engine/world/WorldGenerator.hpp"
#include <tracy/Tracy.hpp>

entt::entity Engine::Chunk::ChunkManager::createChunk(int x, int y, int z) {
  auto chunkPos = Engine::Chunk::ChunkPosition{x, y, z};
  if (getChunk(chunkPos) != entt::null) {
    return entt::null; // Chunk already exists
  }

  entt::entity chunkEntity = m_registry.create();
  m_registry.emplace<Engine::Chunk::ChunkMeshRenderer>(
      chunkEntity, Engine::Render::ShaderManager::get("chunk"));
  m_registry.emplace<Engine::Chunk::ChunkPosition>(chunkEntity, chunkPos);
  m_registry.get_or_emplace<Engine::InUse>(chunkEntity).addUser();

  ThreadPool::getInstance().enqueueTask(
      [this, chunkEntity]() { generateChunkData(chunkEntity); });

  m_chunkMap[chunkPos] = chunkEntity;
  return chunkEntity;
}

void Engine::Chunk::ChunkManager::generateChunkData(entt::entity chunkEntity) {
  if (!m_registry.valid(chunkEntity)) {
    LOG_WARN("Attempted to generate data for invalid chunk entity");
    return;
  }
  auto *chunkPos =
      m_registry.try_get<Engine::Chunk::ChunkPosition>(chunkEntity);
  if (chunkPos == nullptr) {
    LOG_WARN("Attempted to generate data for chunk entity without position");
    return;
  }
  auto chunkData = std::make_shared<Engine::Chunk::ChunkData>();
  m_worldGenerator.generateChunk(chunkPos->x, chunkPos->y, chunkPos->z,
                                 *chunkData);

  m_chunkUpdateQueue.enqueue(std::make_shared<Engine::Chunk::ChunkUpdate>(
      chunkEntity, false, chunkData));
}

void Engine::Chunk::ChunkManager::buildChunkMeshes() {
  ZoneScoped;
  auto view = m_registry.view<Engine::Dirty, Engine::Chunk::ChunkPosition,
                              Engine::Chunk::ChunkData>();
  for (auto entity : view) {
    if (m_registry.all_of<Engine::InUse>(entity)) {
      continue; // Chunk is in use, skip building
    }
    auto &chunkPos = view.get<Engine::Chunk::ChunkPosition>(entity);

    ChunkNeighborhood neighborhood;
    neighborhood.px = nullptr;
    neighborhood.nx = nullptr;
    neighborhood.py = nullptr;
    neighborhood.ny = nullptr;
    neighborhood.pz = nullptr;
    neighborhood.nz = nullptr;

    entt::entity neighborEntity;

    neighborEntity = getChunk(
        Engine::Chunk::ChunkPosition{chunkPos.x + 1, chunkPos.y, chunkPos.z});
    if (neighborEntity != entt::null &&
        m_registry.all_of<ChunkData>(neighborEntity)) {
      neighborhood.px =
          &m_registry.get<Engine::Chunk::ChunkData>(neighborEntity);
    }

    neighborEntity = getChunk(
        Engine::Chunk::ChunkPosition{chunkPos.x - 1, chunkPos.y, chunkPos.z});
    if (neighborEntity != entt::null &&
        m_registry.all_of<ChunkData>(neighborEntity)) {
      neighborhood.nx =
          &m_registry.get<Engine::Chunk::ChunkData>(neighborEntity);
    }

    neighborEntity = getChunk(
        Engine::Chunk::ChunkPosition{chunkPos.x, chunkPos.y + 1, chunkPos.z});
    if (neighborEntity != entt::null &&
        m_registry.all_of<ChunkData>(neighborEntity)) {
      neighborhood.py =
          &m_registry.get<Engine::Chunk::ChunkData>(neighborEntity);
    }

    neighborEntity = getChunk(
        Engine::Chunk::ChunkPosition{chunkPos.x, chunkPos.y - 1, chunkPos.z});
    if (neighborEntity != entt::null &&
        m_registry.all_of<ChunkData>(neighborEntity)) {
      neighborhood.ny =
          &m_registry.get<Engine::Chunk::ChunkData>(neighborEntity);
    }

    neighborEntity = getChunk(
        Engine::Chunk::ChunkPosition{chunkPos.x, chunkPos.y, chunkPos.z + 1});
    if (neighborEntity != entt::null &&
        m_registry.all_of<ChunkData>(neighborEntity)) {
      neighborhood.pz =
          &m_registry.get<Engine::Chunk::ChunkData>(neighborEntity);
    }

    neighborEntity = getChunk(
        Engine::Chunk::ChunkPosition{chunkPos.x, chunkPos.y, chunkPos.z - 1});
    if (neighborEntity != entt::null &&
        m_registry.all_of<ChunkData>(neighborEntity)) {
      neighborhood.nz =
          &m_registry.get<Engine::Chunk::ChunkData>(neighborEntity);
    }

    m_registry.remove<Engine::Dirty>(entity);
    m_registry.emplace<Engine::InUse>(entity).addUser();
    auto &chunkMesh =
        m_registry.get_or_emplace<Engine::Chunk::ChunkMesh>(entity);
    // chunkMesh.vertices.clear();
    // chunkMesh.indices.clear();
    chunkMesh.meshData = std::make_shared<ChunkMeshData>();
    chunkMesh.clean();
    ThreadPool::getInstance().enqueueTask(
        [this, entity, neighborhood, &chunkMesh]() {
          buildChunkMesh(entity, chunkMesh.meshData, neighborhood);
        });
  }
}

void Engine::Chunk::ChunkManager::buildChunkMesh(
    entt::entity chunkEntity, std::shared_ptr<ChunkMeshData> chunkMesh,
    const ChunkNeighborhood &neighborhood) {
  if (!m_registry.valid(chunkEntity)) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Attempted to build mesh for invalid chunk entity");
    return;
  }
  auto &chunkPos = m_registry.get<Engine::Chunk::ChunkPosition>(chunkEntity);
  auto &chunkData = m_registry.get<Engine::Chunk::ChunkData>(chunkEntity);

  auto chunkUpdate = chunkData.buildChunkMesh(neighborhood, chunkMesh);
  // m_chunkUpdateQueue.enqueue(
  //     std::make_shared<Engine::Chunk::ChunkUpdate>(chunkEntity, true,
  //     nullptr));
  m_chunkSetupQueue.enqueue(chunkEntity);
}

void Engine::Chunk::ChunkManager::processChunkUpdates() {
  ZoneScoped;
  std::shared_ptr<Engine::Chunk::ChunkUpdate> chunkUpdate;

  while (m_chunkUpdateQueue.try_dequeue(chunkUpdate)) {
    if (!m_registry.valid(chunkUpdate->entity)) {
      continue; // Entity no longer exists
    }

    // if (chunkUpdate->newMesh == true) {
    //   auto &inUse = m_registry.get<Engine::InUse>(chunkUpdate->entity);
    //   if (inUse.removeUser()) {
    //     m_registry.remove<Engine::InUse>(chunkUpdate->entity);
    //   }
    //   auto &chunkMesh =
    //       m_registry.get<Engine::Chunk::ChunkMesh>(chunkUpdate->entity);
    //   chunkMesh.setupMesh();
    // }

    if (chunkUpdate->newData != nullptr) {
      auto &inUse = m_registry.get<Engine::InUse>(chunkUpdate->entity);
      if (inUse.removeUser()) {
        m_registry.remove<Engine::InUse>(chunkUpdate->entity);
      }
      m_registry.emplace_or_replace<Engine::Chunk::ChunkData>(
          chunkUpdate->entity, *(chunkUpdate->newData));
      if (m_registry.all_of<ChunkPosition>(chunkUpdate->entity)) {
        auto &chunkPos =
            m_registry.get<Engine::Chunk::ChunkPosition>(chunkUpdate->entity);
        for (const auto &neighborPos : chunkPos.neighbors()) {
          tryAddChunkToBuildQueue(neighborPos);
        }
        tryAddChunkToBuildQueue(chunkPos);
      }
    }
  }
  m_chunksSetupThisFrame = 0;
  entt::entity chunkEntity;
  while (m_chunkSetupQueue.try_dequeue(chunkEntity)) {
    if (!m_registry.valid(chunkEntity)) {
      continue; // Entity no longer exists
    }
    auto &inUse = m_registry.get<Engine::InUse>(chunkEntity);
    auto &chunkMesh = m_registry.get<Engine::Chunk::ChunkMesh>(chunkEntity);
    chunkMesh.setupMesh();
    if (inUse.removeUser()) {
      m_registry.remove<Engine::InUse>(chunkEntity);
      // chunkMesh.clean();
    }
    m_chunksSetupThisFrame++;
    if (m_chunksSetupThisFrame >= m_maxChunkSetupsPerFrame) {
      break;
    }
  }
}

void Engine::Chunk::ChunkManager::tryAddChunkToBuildQueue(ChunkPosition pos) {
  entt::entity chunkEntity = getChunk(pos);
  if (chunkEntity == entt::null || !m_registry.valid(chunkEntity)) {
    return; // Chunk does not exist
  }
  if (!m_registry.all_of<Engine::Chunk::ChunkData>(chunkEntity)) {
    return; // Chunk data not generated yet
  }
  if (m_registry.all_of<Engine::Dirty>(chunkEntity)) {
    return; // Chunk is already dirty
  }
  m_registry.emplace<Engine::Dirty>(chunkEntity);
}

bool Engine::Chunk::ChunkManager::deleteOldChunks(int playerX, int playerZ,
                                                  int radius) {
  ZoneScoped;
  auto check = m_registry.view<Engine::InUse>();
  if (check->size() > 0) {
    return false; // Some chunks are still in use, skip deletion
  }
  auto view = m_registry.view<Engine::Chunk::ChunkPosition>();
  std::vector<Engine::Chunk::ChunkPosition> chunksToDelete;
  radius += 1;
  for (auto entity : view) {
    const auto &chunkPos = view.get<Engine::Chunk::ChunkPosition>(entity);
    int dx = chunkPos.x - playerX;
    int dz = chunkPos.z - playerZ;

    if (abs(dx) > radius || abs(dz) > radius) {
      bool unfinishedChunk = false;
      for (const auto &neighborPos : chunkPos.neighbors()) {
        entt::entity neighborEntity = getChunk(neighborPos);
        if (neighborEntity != entt::null && m_registry.valid(neighborEntity)) {
          if (m_registry.all_of<Engine::InUse>(neighborEntity) ||
              m_registry.all_of<Engine::Dirty>(neighborEntity)) {
            unfinishedChunk = true;
            break;
          }
        }
      }
      if (unfinishedChunk)
        continue;
      chunksToDelete.push_back(chunkPos);
    }
  }

  for (const auto &chunkPos : chunksToDelete) {
    deleteChunk(chunkPos.x, chunkPos.y, chunkPos.z);
  }
  return true;
}

void Engine::Chunk::ChunkManager::deleteChunk(int x, int y, int z) {
  auto chunkPos = Engine::Chunk::ChunkPosition{x, y, z};
  entt::entity chunkEntity = getChunk(chunkPos);
  if (chunkEntity == entt::null || !m_registry.valid(chunkEntity)) {
    LOG_WARN("Chunk at (%d, %d, %d) does not exist", x, y, z);
    return; // Chunk does not exist
  }
  if (m_registry.all_of<Engine::Dirty>(chunkEntity) ||
      m_registry.all_of<Engine::InUse>(chunkEntity)) {
    return; // Chunk is dirty, skip deletion
  }

  m_registry.destroy(chunkEntity);
  m_chunkMap.erase(chunkPos);
}

void Engine::Chunk::ChunkManager::loadNewChunks(ChunkPosition playerPos,
                                                int renderDistance) {
  ZoneScoped;
  for (int x = -renderDistance; x <= renderDistance; x++) {
    for (int z = -renderDistance; z <= renderDistance; z++) {
      createChunk(x + playerPos.x, 0, z + playerPos.z);
    }
  }
}

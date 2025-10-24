#include "engine/chunk/ChunkManager.hpp"
#include "core/Application.hpp"
#include "core/Log.hpp"
#include "engine/Camera.hpp"
#include "engine/Engine.hpp"
#include "engine/ThreadPool.hpp"
#include "engine/Transform.hpp"
#include "engine/chunk/ChunkData.hpp"
#include "engine/chunk/ChunkMesh.hpp"
#include "engine/chunk/ChunkPosition.hpp"
#include "engine/voxel/Voxel.hpp"
#include "engine/world/WorldGenerator.hpp"
#include <tracy/Tracy.hpp>

void Engine::Chunk::ChunkManager::init() {
  m_voxelTexture =
      gApp.getTextureManager().loadTexture("assets/textures/VoxelSprites.png");
}

entt::entity Engine::Chunk::ChunkManager::getChunk(
    const Engine::Chunk::ChunkPosition &position) const {
  auto it = m_chunkMap.find(position);
  if (it != m_chunkMap.end() && gApp.getRegistry().valid(it->second)) {
    return it->second;
  }
  return entt::null;
}

Engine::Voxel::Voxel Engine::Chunk::ChunkManager::getBlock(int x, int y,
                                                           int z) const {
  Engine::Chunk::ChunkPosition chunkPos = {x / 32, y / 32, z / 32};
  entt::entity chunkEntity = getChunk(chunkPos);
  if (chunkEntity == entt::null) {
    return {Voxel::VoxelType::Air};
  }
  auto &chunkData =
      gApp.getRegistry().get<Engine::Chunk::ChunkData>(chunkEntity);
  int localX = x % 32;
  int localY = y % 32;
  int localZ = z % 32;
  int index = Engine::Chunk::getIdx(localX, localY, localZ);
  if (index < 0 || index >= CHUNK_SIZE) {
    return {Voxel::VoxelType::Air};
  }
  return chunkData.voxels[index];
}

entt::entity Engine::Chunk::ChunkManager::createChunk(int x, int y, int z) {
  auto chunkPos = Engine::Chunk::ChunkPosition{x, y, z};
  if (getChunk(chunkPos) != entt::null) {
    return entt::null; // Chunk already exists
  }

  entt::entity chunkEntity = gApp.getRegistry().create();
  gApp.getRegistry().emplace<Engine::Chunk::ChunkMeshRenderer>(
      chunkEntity, Engine::Render::ShaderManager::get("chunk"));
  gApp.getRegistry().emplace<Engine::Chunk::ChunkPosition>(chunkEntity,
                                                           chunkPos);
  gApp.getRegistry().get_or_emplace<Engine::InUse>(chunkEntity).addUser();

  ThreadPool::getInstance().enqueueTask(
      [this, chunkEntity]() { generateChunkData(chunkEntity); });

  m_chunkMap[chunkPos] = chunkEntity;
  return chunkEntity;
}

void Engine::Chunk::ChunkManager::generateChunkData(entt::entity chunkEntity) {
  if (!gApp.getRegistry().valid(chunkEntity)) {
    LOG_WARN("Attempted to generate data for invalid chunk entity");
    return;
  }
  auto *chunkPos =
      gApp.getRegistry().try_get<Engine::Chunk::ChunkPosition>(chunkEntity);
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
  auto view = gApp.getRegistry()
                  .view<Engine::Dirty, Engine::Chunk::ChunkPosition,
                        Engine::Chunk::ChunkData>();
  for (auto entity : view) {
    if (gApp.getRegistry().all_of<Engine::InUse>(entity)) {
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
        gApp.getRegistry().all_of<ChunkData>(neighborEntity)) {
      neighborhood.px =
          &gApp.getRegistry().get<Engine::Chunk::ChunkData>(neighborEntity);
    }

    neighborEntity = getChunk(
        Engine::Chunk::ChunkPosition{chunkPos.x - 1, chunkPos.y, chunkPos.z});
    if (neighborEntity != entt::null &&
        gApp.getRegistry().all_of<ChunkData>(neighborEntity)) {
      neighborhood.nx =
          &gApp.getRegistry().get<Engine::Chunk::ChunkData>(neighborEntity);
    }

    neighborEntity = getChunk(
        Engine::Chunk::ChunkPosition{chunkPos.x, chunkPos.y + 1, chunkPos.z});
    if (neighborEntity != entt::null &&
        gApp.getRegistry().all_of<ChunkData>(neighborEntity)) {
      neighborhood.py =
          &gApp.getRegistry().get<Engine::Chunk::ChunkData>(neighborEntity);
    }

    neighborEntity = getChunk(
        Engine::Chunk::ChunkPosition{chunkPos.x, chunkPos.y - 1, chunkPos.z});
    if (neighborEntity != entt::null &&
        gApp.getRegistry().all_of<ChunkData>(neighborEntity)) {
      neighborhood.ny =
          &gApp.getRegistry().get<Engine::Chunk::ChunkData>(neighborEntity);
    }

    neighborEntity = getChunk(
        Engine::Chunk::ChunkPosition{chunkPos.x, chunkPos.y, chunkPos.z + 1});
    if (neighborEntity != entt::null &&
        gApp.getRegistry().all_of<ChunkData>(neighborEntity)) {
      neighborhood.pz =
          &gApp.getRegistry().get<Engine::Chunk::ChunkData>(neighborEntity);
    }

    neighborEntity = getChunk(
        Engine::Chunk::ChunkPosition{chunkPos.x, chunkPos.y, chunkPos.z - 1});
    if (neighborEntity != entt::null &&
        gApp.getRegistry().all_of<ChunkData>(neighborEntity)) {
      neighborhood.nz =
          &gApp.getRegistry().get<Engine::Chunk::ChunkData>(neighborEntity);
    }

    gApp.getRegistry().remove<Engine::Dirty>(entity);
    gApp.getRegistry().emplace<Engine::InUse>(entity).addUser();
    auto &chunkMesh =
        gApp.getRegistry().get_or_emplace<Engine::Chunk::ChunkMesh>(entity);
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
  if (!gApp.getRegistry().valid(chunkEntity)) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "Attempted to build mesh for invalid chunk entity");
    return;
  }
  auto &chunkPos =
      gApp.getRegistry().get<Engine::Chunk::ChunkPosition>(chunkEntity);
  auto &chunkData =
      gApp.getRegistry().get<Engine::Chunk::ChunkData>(chunkEntity);

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
    if (!gApp.getRegistry().valid(chunkUpdate->entity)) {
      continue; // Entity no longer exists
    }

    // if (chunkUpdate->newMesh == true) {
    //   auto &inUse =
    //   gApp.getRegistry().get<Engine::InUse>(chunkUpdate->entity); if
    //   (inUse.removeUser()) {
    //     gApp.getRegistry().remove<Engine::InUse>(chunkUpdate->entity);
    //   }
    //   auto &chunkMesh =
    //       gApp.getRegistry().get<Engine::Chunk::ChunkMesh>(chunkUpdate->entity);
    //   chunkMesh.setupMesh();
    // }

    if (chunkUpdate->newData != nullptr) {
      auto &inUse = gApp.getRegistry().get<Engine::InUse>(chunkUpdate->entity);
      if (inUse.removeUser()) {
        gApp.getRegistry().remove<Engine::InUse>(chunkUpdate->entity);
      }
      gApp.getRegistry().emplace_or_replace<Engine::Chunk::ChunkData>(
          chunkUpdate->entity, *(chunkUpdate->newData));
      if (gApp.getRegistry().all_of<ChunkPosition>(chunkUpdate->entity)) {
        auto &chunkPos = gApp.getRegistry().get<Engine::Chunk::ChunkPosition>(
            chunkUpdate->entity);
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
    if (!gApp.getRegistry().valid(chunkEntity)) {
      continue; // Entity no longer exists
    }
    auto &inUse = gApp.getRegistry().get<Engine::InUse>(chunkEntity);
    auto &chunkMesh =
        gApp.getRegistry().get<Engine::Chunk::ChunkMesh>(chunkEntity);
    chunkMesh.setupMesh();
    if (inUse.removeUser()) {
      gApp.getRegistry().remove<Engine::InUse>(chunkEntity);
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
  if (chunkEntity == entt::null || !gApp.getRegistry().valid(chunkEntity)) {
    return; // Chunk does not exist
  }
  if (!gApp.getRegistry().all_of<Engine::Chunk::ChunkData>(chunkEntity)) {
    return; // Chunk data not generated yet
  }
  if (gApp.getRegistry().all_of<Engine::Dirty>(chunkEntity)) {
    return; // Chunk is already dirty
  }
  gApp.getRegistry().emplace<Engine::Dirty>(chunkEntity);
}

bool Engine::Chunk::ChunkManager::deleteOldChunks(int playerX, int playerZ,
                                                  int radius) {
  ZoneScoped;
  auto check = gApp.getRegistry().view<Engine::InUse>();
  if (check->size() > 0) {
    return false; // Some chunks are still in use, skip deletion
  }
  auto view = gApp.getRegistry().view<Engine::Chunk::ChunkPosition>();
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
        if (neighborEntity != entt::null &&
            gApp.getRegistry().valid(neighborEntity)) {
          if (gApp.getRegistry().all_of<Engine::InUse>(neighborEntity) ||
              gApp.getRegistry().all_of<Engine::Dirty>(neighborEntity)) {
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
  if (chunkEntity == entt::null || !gApp.getRegistry().valid(chunkEntity)) {
    LOG_WARN("Chunk at ({}, {}, {}) does not exist", x, y, z);
    return; // Chunk does not exist
  }
  if (gApp.getRegistry().all_of<Engine::Dirty>(chunkEntity) ||
      gApp.getRegistry().all_of<Engine::InUse>(chunkEntity)) {
    return; // Chunk is dirty, skip deletion
  }

  gApp.getRegistry().destroy(chunkEntity);
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

void Engine::Chunk::ChunkManager::update() {
  processChunkUpdates();
  buildChunkMeshes();

  auto playerChunkPos = gApp.getRegistry()
                            .get<Engine::Transform>(gApp.getActiveCamera())
                            .getChunkPosition();

  auto chunksDeleted =
      deleteOldChunks(playerChunkPos.x, playerChunkPos.z, m_renderDistance);
  if (chunksDeleted)
    loadNewChunks(playerChunkPos, m_renderDistance);
}

void Engine::Chunk::ChunkManager::render(const glm::mat4 &viewProjection) {
  ZoneScoped;
  auto renderedChunksView =
      gApp.getRegistry().view<ChunkMesh, ChunkPosition, ChunkMeshRenderer>();

  auto &camera = gApp.getRegistry().get<Engine::Camera>(gApp.getActiveCamera());
  auto &cameraTransform =
      gApp.getRegistry().get<Engine::Transform>(gApp.getActiveCamera());

  const Engine::Render::Shader &chunkShader =
      Engine::Render::ShaderManager::get("chunk");
  chunkShader.use();
  chunkShader.setMat4("u_ViewProjection", viewProjection);
  m_voxelTexture->bind(0);

  for (auto entity : renderedChunksView) {
    auto &mesh = renderedChunksView.get<Engine::Chunk::ChunkMesh>(entity);

    if (!gApp.getRegistry().all_of<Engine::Dirty>(entity) &&
        !gApp.getRegistry().all_of<Engine::InUse>(entity)) {
      mesh.clean();
    }

    auto &position = renderedChunksView.get<ChunkPosition>(entity);

    auto fPos = position.toWorldPosition() - cameraTransform.position +
                (camera.front * static_cast<float>(CHUNK_WIDTH) / 2.0f);

    auto fPosNorm = glm::normalize(fPos);
    float dot = glm::dot(camera.front, fPosNorm);
    float threshold = 0.0f;
    if (dot < threshold) {
      continue;
    }

    auto &renderer = renderedChunksView.get<Chunk::ChunkMeshRenderer>(entity);
    renderer.render(mesh, position);
  }
}

#pragma once
#include "engine/chunk/ChunkData.hpp"
#include "engine/chunk/ChunkMesh.hpp"
#include <entt/entt.hpp>
namespace Engine::Chunk {
struct ChunkUpdate {
  entt::entity entity;
  std::shared_ptr<ChunkMesh> newMesh;
  std::shared_ptr<ChunkData> newData;
  unsigned int lastUpdateTime = 0;
};
} // namespace Engine::Chunk

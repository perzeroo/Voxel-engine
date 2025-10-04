#pragma once
#include "engine/chunk/ChunkMesh.hpp"
#include <entt/entt.hpp>
namespace Engine::Chunk {
struct ChunkUpdate {
  entt::entity entity;
  std::shared_ptr<ChunkMesh> newMesh;
};
}

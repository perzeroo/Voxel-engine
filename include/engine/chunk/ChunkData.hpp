#pragma once
#include "core/Common.hpp"
#include "engine/voxel/Voxel.hpp"
#include <array>
namespace Engine::Chunk {
struct ChunkData {
  std::array<Engine::Voxel::Voxel, CHUNK_SIZE> voxels;
};
}

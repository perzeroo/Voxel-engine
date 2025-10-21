#pragma once
#include "engine/voxel/Voxel.hpp"
#include "glm/glm.hpp"
#include <cstdint>

namespace Engine::Texture {
  uint8_t getTileIndex(Voxel::VoxelType voxelType, uint8_t faceIndex);
  glm::vec2 getUV(uint8_t tileIndex, glm::vec2 corner, int tilesPerRow = 16);
}

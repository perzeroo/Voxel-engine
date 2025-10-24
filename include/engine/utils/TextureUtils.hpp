#pragma once

#include <cstdint>
#include "engine/voxel/Voxel.hpp"
#include "engine/Render.hpp"

namespace Engine::Utils {
inline uint8_t getTileIndex(Engine::Voxel::VoxelType voxelType,
                     Engine::Render::FaceDirection faceIndex) {
  switch (voxelType) {
  case Engine::Voxel::VoxelType::Dirt:
    return 2;
  case Engine::Voxel::VoxelType::Grass:
    if (faceIndex == Render::FaceDirection::TOP) {
      return 0;
    } else if (faceIndex == Render::FaceDirection::BOTTOM) {
      return 2;
    } else {
      return 1;
    }
  default:
    return 0;
  }
}
inline glm::vec2 getUV(uint8_t tileIndex, glm::vec2 corner, int tilesPerRow = 16) {
  int x = tileIndex % tilesPerRow;
  int y = tileIndex / tilesPerRow;
  float tileSize = 1.0f / static_cast<float>(tilesPerRow);
  float padding = 0.001f * tileSize;
  glm::vec2 uvMin = glm::vec2(x, y) * tileSize + glm::vec2(padding);
  glm::vec2 uvMax = glm::vec2(x + 1, y + 1) * tileSize - glm::vec2(padding);
  // glm::vec2 uvMax = glm::vec2(x * tileSize + 1, y * tileSize + 1) - (0.001f *
  // tileSize); return glm::vec2(x * tileSize, y * tileSize) + corner *
  // tileSize;
  return glm::mix(uvMin, uvMax, corner);
}
}

#include "engine/Texture.hpp"

uint8_t Engine::Texture::getTileIndex(Engine::Voxel::VoxelType voxelType, uint8_t faceIndex) {
  switch (voxelType) {
    case Engine::Voxel::VoxelType::Dirt:
      return 2;
    case Engine::Voxel::VoxelType::Grass:
      if (faceIndex == 4) {
        return 0;
      } else if (faceIndex == 5) {
        return 2;
      } else {
        return 1;
      }
    default:
      return 0;
  }
}

glm::vec2 Engine::Texture::getUV(uint8_t tileIndex, glm::vec2 corner, int tilesPerRow) {
  int x = tileIndex % tilesPerRow;
  int y = tileIndex / tilesPerRow;
  float tileSize = 1.0f / static_cast<float>(tilesPerRow);

  return glm::vec2(x * tileSize, y * tileSize) + corner * tileSize;
}

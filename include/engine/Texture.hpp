#pragma once
#include "engine/voxel/Voxel.hpp"
#include "engine/Render.hpp"
#include "glm/glm.hpp"
#include <cstdint>

namespace Engine::Texture {
  uint8_t getTileIndex(Voxel::VoxelType voxelType, Engine::Render::FaceDirection faceIndex);
  glm::vec2 getUV(uint8_t tileIndex, glm::vec2 corner, int tilesPerRow = 16);
}
// namespace Engine {
//   class TextureManager{
//   public:
//     TextureManager();
//     TextureManager(TextureManager &&) = default;
//     TextureManager(const TextureManager &) = default;
//     TextureManager &operator=(TextureManager &&) = default;
//     TextureManager &operator=(const TextureManager &) = default;
//     ~TextureManager();
//
//   private:
//
//   };
//
// }

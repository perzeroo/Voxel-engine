#pragma once
#include "engine/Render.hpp"
#include "engine/voxel/Voxel.hpp"
#include "glm/glm.hpp"
#include <cstdint>

namespace Engine::Texture {
uint8_t getTileIndex(Voxel::VoxelType voxelType,
                     Engine::Render::FaceDirection faceIndex);
glm::vec2 getUV(uint8_t tileIndex, glm::vec2 corner, int tilesPerRow = 16);

class TextureManager {
public:
  TextureManager();
  TextureManager(TextureManager &&) = default;
  TextureManager(const TextureManager &) = default;
  TextureManager &operator=(TextureManager &&) = default;
  TextureManager &operator=(const TextureManager &) = default;
  ~TextureManager();
  void useVoxelTexture() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_voxelTextureID);
  }

private:
  GLuint m_voxelTextureID;
};
} // namespace Engine::Texture
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

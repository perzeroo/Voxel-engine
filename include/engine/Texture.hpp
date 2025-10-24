#pragma once
#include "engine/Render.hpp"
#include "engine/voxel/Voxel.hpp"
#include "glm/glm.hpp"
#include <cstdint>

namespace Engine {
class Texture {
public:
  Texture();
  Texture(Texture &&) = default;
  Texture(const Texture &) = default;
  Texture &operator=(Texture &&) = default;
  Texture &operator=(const Texture &) = default;
  ~Texture();

private:
  GLuint m_textureID;
};

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

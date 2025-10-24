#pragma once
#include "engine/Render.hpp"
#include "glm/glm.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace Engine {
class Texture {
public:
  Texture(const std::string &path);
  Texture(Texture &&) = default;
  Texture(const Texture &) = default;
  Texture &operator=(Texture &&) = default;
  Texture &operator=(const Texture &) = default;
  ~Texture();
  void bind(uint32_t slot = 0) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
  }

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
  Texture &loadTexture(const std::string &path) {
    m_textures.emplace_back(path);
    return m_textures.back();
  }
  void useVoxelTexture() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_voxelTextureID);
  }

private:
  std::vector<Engine::Texture> m_textures;
  GLuint m_voxelTextureID;
};
} // namespace Engine

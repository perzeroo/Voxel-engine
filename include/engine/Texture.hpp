#pragma once
#include "engine/Render.hpp"
#include "glm/glm.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace Engine {
class Texture {
public:
  Texture(const std::string &path, uint32_t id);
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
  uint32_t m_id;
};

class TextureManager {
public:
  TextureManager() = default;
  TextureManager(TextureManager &&) = default;
  TextureManager(const TextureManager &) = default;
  TextureManager &operator=(TextureManager &&) = default;
  TextureManager &operator=(const TextureManager &) = default;
  ~TextureManager() = default;
  std::shared_ptr<Texture> loadTexture(const std::string &path) {
    auto tx = std::make_shared<Texture>(path, 0);
    m_textures.push_back(tx);
    return tx;
  }

private:
  std::vector<std::weak_ptr<Texture>> m_textures;
};
} // namespace Engine

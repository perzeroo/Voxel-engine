#include "engine/Texture.hpp"
#include "core/Log.hpp"
#include "engine/Render.hpp"
#include "glm/common.hpp"
#include "thirdparty/stb/stb_image.hpp"


Engine::Texture::TextureManager::TextureManager() {
  int width, height, channels;
  unsigned char *imageData =
      stbi_load("assets/textures/VoxelSprites.png", &width, &height, &channels,
                4); // 4 = force RGBA

  if (!imageData) {
    LOG_ERROR("Failed to load texture: assets/textures/VoxelSprites.png");
    m_voxelTextureID = 0;
    return;
  }

  glGenTextures(1, &m_voxelTextureID);
  glBindTexture(GL_TEXTURE_2D, m_voxelTextureID);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, imageData);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  stbi_image_free(imageData);
}

Engine::Texture::TextureManager::~TextureManager() {
  glDeleteTextures(1, &m_voxelTextureID);
}

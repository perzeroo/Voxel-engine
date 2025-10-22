#include "engine/Texture.hpp"
#include "SDL3/SDL_log.h"
#include "engine/Render.hpp"
#include "glm/common.hpp"
#include "thirdparty/stb/stb_image.hpp"

uint8_t Engine::Texture::getTileIndex(Engine::Voxel::VoxelType voxelType,
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

glm::vec2 Engine::Texture::getUV(uint8_t tileIndex, glm::vec2 corner,
                                 int tilesPerRow) {
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

Engine::Texture::TextureManager::TextureManager() {
  int width, height, channels;
  unsigned char *imageData =
      stbi_load("assets/textures/VoxelSprites.png", &width, &height, &channels,
                4); // 4 = force RGBA

  if (!imageData) {
    SDL_Log("Failed to load texture: assets/textures/VoxelSprites.png");
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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  stbi_image_free(imageData);
}

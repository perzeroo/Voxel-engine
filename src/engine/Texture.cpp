#include "engine/Texture.hpp"
#include "core/Log.hpp"
#include "engine/Render.hpp"
#include "glm/common.hpp"
#include "thirdparty/stb/stb_image.hpp"

Engine::Texture::Texture(const std::string &path, uint32_t id) : m_id(id) {
  int width, height, channels;
  unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);
  if (!data) {
    LOG_ERROR("Failed to load texture: {}", path);
    m_textureID = 0;
    return;
  }

  glGenTextures(1, &m_textureID);
  glBindTexture(GL_TEXTURE_2D, m_textureID);

  GLenum format = GL_RGB;
  switch (channels) {
  case 1:
    format = GL_RED;
    break;
  case 3:
    format = GL_RGB;
    break;
  case 4:
    format = GL_RGBA;
    break;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);
}
Engine::Texture::~Texture() {
  if (m_textureID != 0) {
    glDeleteTextures(1, &m_textureID);
  }
}

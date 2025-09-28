#pragma once
#include "glm/glm.hpp"

namespace Engine::Render {
const glm::vec3 FACE_NORMALS[6] = {
  glm::vec3( 0,  0,  1), // Front
  glm::vec3( 0,  0, -1), // Back
  glm::vec3( 1,  0,  0), // Right
  glm::vec3(-1,  0,  0), // Left
  glm::vec3( 0,  1,  0), // Top
  glm::vec3( 0, -1,  0)  // Bottom
};
  struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
  };
}

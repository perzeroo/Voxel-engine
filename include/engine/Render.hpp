#pragma once
#include "glm/glm.hpp"
#include <cstdint>
#include <glad/glad.h>

namespace Engine::Render {
enum FaceDirection {
  FRONT = 0,
  BACK = 1,
  RIGHT = 2,
  LEFT = 3,
  TOP = 4,
  BOTTOM = 5
};
const glm::vec3 FACE_NORMALS[6] = {
    glm::vec3(0, 0, 1),  // Front
    glm::vec3(0, 0, -1), // Back
    glm::vec3(1, 0, 0),  // Right
    glm::vec3(-1, 0, 0), // Left
    glm::vec3(0, 1, 0),  // Top
    glm::vec3(0, -1, 0)  // Bottom
};

const glm::vec3 FACE_VERTEX_POSITIONS[6][4] = {
    // Front face
    {glm::vec3(0, 0, 1), glm::vec3(1, 0, 1), glm::vec3(1, 1, 1),
     glm::vec3(0, 1, 1)},
    // Back face
    {glm::vec3(1, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0),
     glm::vec3(1, 1, 0)},
    // Right face
    {glm::vec3(1, 0, 1), glm::vec3(1, 0, 0), glm::vec3(1, 1, 0),
     glm::vec3(1, 1, 1)},
    // Left face
    {glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 1),
     glm::vec3(0, 1, 0)},
    // Top face
    {glm::vec3(0, 1, 1), glm::vec3(1, 1, 1), glm::vec3(1, 1, 0),
     glm::vec3(0, 1, 0)},
    // Bottom face
    {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 1),
     glm::vec3(0, 0, 1)}};

const glm::vec2 FACE_UV_COORDS[4] = {
    glm::vec2(0.0f, 1.0f), // Top-left
    glm::vec2(1.0f, 1.0f),  // Top-right
    glm::vec2(1.0f, 0.0f), // Bottom-right
    glm::vec2(0.0f, 0.0f), // Bottom-left
};

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
  uint8_t type;
};
const int FACE_INDICES[6][6] = {
    {0, 1, 2, 0, 2, 3},
    {0, 2, 1, 0, 3, 2},
    {0, 1, 2, 0, 2, 3},
    {0, 2, 1, 0, 3, 2},
    {0, 1, 2, 0, 2, 3},
    {0, 2, 1, 0, 3, 2}, 
};

} // namespace Engine::Render

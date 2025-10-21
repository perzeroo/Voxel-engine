#pragma once
#include "glm/glm.hpp"
#include <cstdint>
#include <glad/glad.h>

namespace Engine::Render {
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

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
  uint8_t type;
};

// const Vertex FACE_VERTICES[6][4] = {
//     // Front face
//     {FACE_VERTEX_POSITIONS[0][0], FACE_NORMALS[0], FACE_VERTEX_POSITIONS[0][1],
//      FACE_NORMALS[0], FACE_VERTEX_POSITIONS[0][2], FACE_NORMALS[0],
//      FACE_VERTEX_POSITIONS[0][3], FACE_NORMALS[0]},
//     // Back face
//     {FACE_VERTEX_POSITIONS[1][0], FACE_NORMALS[1], FACE_VERTEX_POSITIONS[1][1],
//      FACE_NORMALS[1], FACE_VERTEX_POSITIONS[1][2], FACE_NORMALS[1],
//      FACE_VERTEX_POSITIONS[1][3], FACE_NORMALS[1]},
//     // Right face
//     {FACE_VERTEX_POSITIONS[2][0], FACE_NORMALS[2], FACE_VERTEX_POSITIONS[2][1],
//      FACE_NORMALS[2], FACE_VERTEX_POSITIONS[2][2], FACE_NORMALS[2],
//      FACE_VERTEX_POSITIONS[2][3], FACE_NORMALS[2]},
//     // Left face
//     {FACE_VERTEX_POSITIONS[3][0], FACE_NORMALS[3], FACE_VERTEX_POSITIONS[3][1],
//      FACE_NORMALS[3], FACE_VERTEX_POSITIONS[3][2], FACE_NORMALS[3],
//      FACE_VERTEX_POSITIONS[3][3], FACE_NORMALS[3]},
//     // Top face
//     {FACE_VERTEX_POSITIONS[4][0], FACE_NORMALS[4], FACE_VERTEX_POSITIONS[4][1],
//      FACE_NORMALS[4], FACE_VERTEX_POSITIONS[4][2], FACE_NORMALS[4],
//      FACE_VERTEX_POSITIONS[4][3], FACE_NORMALS[4]},
//     // Bottom face
//     {FACE_VERTEX_POSITIONS[5][0], FACE_NORMALS[5], FACE_VERTEX_POSITIONS[5][1],
//      FACE_NORMALS[5], FACE_VERTEX_POSITIONS[5][2], FACE_NORMALS[5],
//      FACE_VERTEX_POSITIONS[5][3], FACE_NORMALS[5]}};

// const int FACE_INDICES[6][6] = {
//   {0, 1, 2, 0, 2, 3}, // Front
//   {4, 6, 5, 4, 7, 6}, // Back
//   {8, 9,10, 8,10,11}, // Right
//   {12,14,13,12,15,14}, // Left
//   {16,17,18,16,18,19}, // Top
//   {20,22,21,20,23,22}  // Bottom
// };
const int FACE_INDICES[6][6] = {
    {0, 1, 2, 0, 2, 3}, // Front
    {0, 2, 1, 0, 3, 2}, // Front
    {0, 1, 2, 0, 2, 3}, // Front
    {0, 2, 1, 0, 3, 2}, // Front
    {0, 1, 2, 0, 2, 3}, // Front
    {0, 2, 1, 0, 3, 2}, // Front
};

} // namespace Engine::Render

#pragma once

#include <glad/glad.h>
#include "engine/Render.hpp"
#include <vector>

namespace Engine::Chunk {
struct ChunkMesh {
  std::vector<Engine::Render::Vertex> vertices;
  std::vector<unsigned int> indices; 
  GLuint VAO = 0;
  GLuint VBO = 0;
  GLuint EBO = 0;
  void setupMesh();
  void removeMesh();
};
} 

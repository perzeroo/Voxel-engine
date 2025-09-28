#include <engine/chunk/ChunkMesh.hpp>

void Engine::Chunk::ChunkMesh::setupMesh() {
  if (VAO == 0) {
    glGenVertexArrays(1, &VAO);
  }
  if (VBO == 0) {
    glGenBuffers(1, &VBO);
  }
  if (EBO == 0) {
    glGenBuffers(1, &EBO);
  }

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Render::Vertex), vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

  // Vertex positions
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Render::Vertex), (void*)offsetof(Render::Vertex, position));
  glEnableVertexAttribArray(0);
  // Vertex normals
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Render::Vertex), (void*)offsetof(Render::Vertex, normal));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

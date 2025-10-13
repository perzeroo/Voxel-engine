#include <engine/chunk/ChunkMesh.hpp>

void Engine::Chunk::ChunkMesh::on_destroy(entt::registry &registry,
                                          entt::entity entity) {
  if (registry.all_of<ChunkMesh>(entity)) {
    auto &mesh = registry.get<ChunkMesh>(entity);
    mesh.removeMesh();
    mesh.vertices.clear();
    mesh.indices.clear();
    mesh.vertices.shrink_to_fit();
    mesh.indices.shrink_to_fit();
  }
}

void Engine::Chunk::ChunkMesh::setupMesh() {
  removeMesh();
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
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Render::Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  // Vertex positions
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Render::Vertex),
                        (void *)offsetof(Render::Vertex, position));
  glEnableVertexAttribArray(0);
  // Vertex normals
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Render::Vertex),
                        (void *)offsetof(Render::Vertex, normal));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  indicesSize = static_cast<GLsizei>(indices.size());
}

void Engine::Chunk::ChunkMesh::removeMesh() {
  if (EBO != 0) {
    glDeleteBuffers(1, &EBO);
    EBO = 0;
  }
  if (VBO != 0) {
    glDeleteBuffers(1, &VBO);
    VBO = 0;
  }
  if (VAO != 0) {
    glDeleteVertexArrays(1, &VAO);
    VAO = 0;
  }
}

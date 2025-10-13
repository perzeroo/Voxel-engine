#pragma once

#include "SDL3/SDL_log.h"
#include "core/Common.hpp"
#include "engine/Render.hpp"
#include "engine/ShaderManager.hpp"
#include "engine/chunk/ChunkPosition.hpp"
#include "entt/entt.hpp"
#include <glad/glad.h>
#include <vector>

namespace Engine::Chunk {
struct ChunkMesh {
  std::vector<Engine::Render::Vertex> vertices;
  std::vector<unsigned int> indices;
  GLuint VAO = 0;
  GLuint VBO = 0;
  GLuint EBO = 0;
  GLsizei indicesSize = 0;
  void setupMesh();
  void removeMesh();
  static void on_destroy(entt::registry &registry, entt::entity entity);
};
struct ChunkMeshRenderer {
  Engine::Render::Shader &shader;
  void render(ChunkMesh &mesh, const ChunkPosition &position,
              const glm::mat4 &viewProjection) {
    if (mesh.VAO == 0) {
      // mesh.setupMesh();
      return; // Nothing to render
    }
    // shader.use();
    glm::mat4 model =
        glm::translate(glm::mat4(1.0f), glm::vec3(position.x * CHUNK_WIDTH,
                                                  position.y * CHUNK_WIDTH,
                                                  position.z * CHUNK_WIDTH));
    shader.setMat4("u_Model", model);
    // shader.setMat4("u_ViewProjection", viewProjection);
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indicesSize,
                   GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
};
} // namespace Engine::Chunk

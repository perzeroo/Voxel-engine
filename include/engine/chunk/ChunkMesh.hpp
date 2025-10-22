#pragma once

#include "SDL3/SDL_log.h"
#include "core/Common.hpp"
#include "engine/Render.hpp"
#include "engine/ShaderManager.hpp"
#include "engine/chunk/ChunkPosition.hpp"
#include "engine/voxel/Voxel.hpp"
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
  void setupMesh(bool isDirty = false);
  void removeMesh();
  void addFrontFace(float x, float y, float z, Engine::Voxel::Voxel type);
  void addBackFace(float x, float y, float z, Engine::Voxel::Voxel type);
  void addRightFace(float x, float y, float z, Engine::Voxel::Voxel type);
  void addLeftFace(float x, float y, float z, Engine::Voxel::Voxel type);
  void addTopFace(float x, float y, float z, Engine::Voxel::Voxel type);
  void addBottomFace(float x, float y, float z, Engine::Voxel::Voxel type);
  void addFaceFromIndex(int faceIndex, float x, float y, float z,
                        Engine::Voxel::Voxel type);
  void clean();
  static void on_destroy(entt::registry &registry, entt::entity entity);
};
struct ChunkMeshRenderer {
  Engine::Render::Shader &shader;
  void render(ChunkMesh &mesh, const ChunkPosition &position,
              const glm::mat4 &viewProjection) {
    if (mesh.VAO == 0) {
      return; // Nothing to render
    }
    // shader.use();
    glm::mat4 model =
        glm::translate(glm::mat4(1.0f), position.toWorldPosition());
    shader.setMat4("u_Model", model);
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indicesSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
};
} // namespace Engine::Chunk

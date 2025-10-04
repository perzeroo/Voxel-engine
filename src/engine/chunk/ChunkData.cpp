#include "engine/Engine.hpp"
#include "engine/chunk/ChunkMesh.hpp"
#include <engine/chunk/ChunkData.hpp>
#include <engine/chunk/ChunkMeshBuilder.hpp>
#include <memory>

void Engine::Chunk::ChunkData::on_update(entt::registry &registry, entt::entity entity) {
  if (registry.all_of<ChunkMeshBuilder>(entity)) {
    // ChunkMeshBuilder::on_chunk_data_change(registry, entity);
    registry.emplace<Engine::Dirty>(entity);
  }
}

std::shared_ptr<Engine::Chunk::ChunkMesh> Engine::Chunk::ChunkData::buildChunkMesh() {
  auto chunkMesh = std::make_shared<ChunkMesh>();
  
  chunkMesh->vertices.clear();
  chunkMesh->indices.clear();

  for (int i = 0; i < CHUNK_SIZE; i++) {
    auto voxel_type = voxels[i].type;
    if (voxel_type == 0) {
      continue; // Skip empty voxels
    }

    int z = i / (CHUNK_WIDTH * CHUNK_WIDTH);
    int y = (i % (CHUNK_WIDTH * CHUNK_WIDTH)) / CHUNK_WIDTH;
    int x = i % CHUNK_WIDTH;

    float fx = static_cast<float>(x);
    float fy = static_cast<float>(y);
    float fz = static_cast<float>(z);

    unsigned int baseIndex =
        static_cast<unsigned int>(chunkMesh->vertices.size());

    if (z == CHUNK_WIDTH - 1 ||
        voxels[getIdx(x, y, z + 1)].type == 0) {
      // Front face
      for (int i = 0; i < 6; ++i) {
        if (i < 4) {
          Render::Vertex vert = Render::FACE_VERTICES[0][i];
          vert.position += glm::vec3(fx, fy, fz);
          chunkMesh->vertices.push_back(vert);
        }
        chunkMesh->indices.push_back(Render::FACE_INDICES[0][i] + baseIndex);
      }
      baseIndex += 4;
    }
    if (z == 0 || voxels[getIdx(x, y, z - 1)].type == 0) {
      // Back face
      for (int i = 0; i < 6; ++i) {
        if (i < 4) {
          Render::Vertex vert = Render::FACE_VERTICES[1][i];
          vert.position += glm::vec3(fx, fy, fz);
          chunkMesh->vertices.push_back(vert);
        }
        chunkMesh->indices.push_back(Render::FACE_INDICES[1][i] + baseIndex);
      }
      baseIndex += 4;
    }
    if (x == CHUNK_WIDTH - 1 ||
        voxels[getIdx(x + 1, y, z)].type == 0) {
      // Right face
      for (int i = 0; i < 6; ++i) {
        if (i < 4) {
          Render::Vertex vert = Render::FACE_VERTICES[2][i];
          vert.position += glm::vec3(fx, fy, fz);
          chunkMesh->vertices.push_back(vert);
        }
        chunkMesh->indices.push_back(Render::FACE_INDICES[2][i] + baseIndex);
      }
      baseIndex += 4;
    }
    if (x == 0 || voxels[getIdx(x - 1, y, z)].type == 0) {
      // Left face
      for (int i = 0; i < 6; ++i) {
        if (i < 4) {
          Render::Vertex vert = Render::FACE_VERTICES[3][i];
          vert.position += glm::vec3(fx, fy, fz);
          chunkMesh->vertices.push_back(vert);
        }
        chunkMesh->indices.push_back(Render::FACE_INDICES[3][i] + baseIndex);
      }
      baseIndex += 4;
    }
    if (y == CHUNK_WIDTH - 1 ||
        voxels[getIdx(x, y + 1, z)].type == 0) {
      // Top face
      for (int i = 0; i < 6; ++i) {
        if (i < 4) {
          Render::Vertex vert = Render::FACE_VERTICES[4][i];
          vert.position += glm::vec3(fx, fy, fz);
          chunkMesh->vertices.push_back(vert);
        }
        chunkMesh->indices.push_back(Render::FACE_INDICES[4][i] + baseIndex);
      }
      baseIndex += 4;
    }
    if (y == 0 || voxels[getIdx(x, y - 1, z)].type == 0) {
      // Bottom face
      for (int i = 0; i < 6; ++i) {
        if (i < 4) {
          Render::Vertex vert = Render::FACE_VERTICES[5][i];
          vert.position += glm::vec3(fx, fy, fz);
          chunkMesh->vertices.push_back(vert);
        }
        chunkMesh->indices.push_back(Render::FACE_INDICES[5][i] + baseIndex);
      }
      baseIndex += 4;
    }
  }
  chunkMesh->setupMesh();
  return chunkMesh;
}

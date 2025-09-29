#include "SDL3/SDL_log.h"
#include "SDL3/SDL_timer.h"
#include "core/Common.hpp"
#include "engine/Render.hpp"
#include "engine/chunk/ChunkData.hpp"
#include "engine/chunk/ChunkManager.hpp"
#include <engine/chunk/ChunkMeshBuilder.hpp>
#include <iterator>

void Engine::Chunk::ChunkMeshBuilder::on_construct(entt::registry &registry,
                                                   entt::entity entity) {
  Engine::Chunk::ChunkMeshBuilder::on_chunk_data_change(registry, entity);
}

void Engine::Chunk::ChunkMeshBuilder::on_chunk_data_change(
    entt::registry &registry, entt::entity entity) {
  auto &chunkData = registry.get<ChunkData>(entity);
  if (!registry.all_of<ChunkMesh>(entity)) {
    registry.emplace<ChunkMesh>(entity);
  }
  auto &chunkMesh = registry.get<ChunkMesh>(entity);
  build(chunkData, chunkMesh);
}

void Engine::Chunk::ChunkMeshBuilder::build(ChunkData &chunkData,
                                            ChunkMesh &chunkMesh) {
  if (Engine::Chunk::ChunkManager::CHUNKS_GENERATED_THIS_FRAME++ > 16) {
    // return; // Limit to 16 chunks per frame
  }
  unsigned int startTime = SDL_GetTicks();
  SDL_Log("Building chunk mesh...");

  chunkMesh.vertices.clear();
  chunkMesh.indices.clear();

  for (int i = 0; i < CHUNK_SIZE; i++) {
    auto voxel_type = chunkData.voxels[i].type;
    if (voxel_type == 0) {
      continue; // Skip empty voxels
    }

    int z = i / (CHUNK_WIDTH * CHUNK_WIDTH);
    int y = (i % (CHUNK_WIDTH * CHUNK_WIDTH)) / CHUNK_WIDTH;
    int x = i % CHUNK_WIDTH;

    float fx = static_cast<float>(x);
    float fy = static_cast<float>(y);
    float fz = static_cast<float>(z);
    // if (getIdx(x, y, z) != i) {
    //   SDL_Log("Indexing error at %d, %d, %d: got %d", x, y, z, getIdx(x, y,
    //   z));
    // }

    unsigned int baseIndex =
        static_cast<unsigned int>(chunkMesh.vertices.size());

    if (z == CHUNK_WIDTH - 1 ||
        chunkData.voxels[getIdx(x, y, z + 1)].type == 0) {
      // Front face
      for (int i = 0; i < 6; ++i) {
        if (i < 4) {
          Render::Vertex vert = Render::FACE_VERTICES[0][i];
          vert.position += glm::vec3(fx, fy, fz);
          chunkMesh.vertices.push_back(vert);
        }
        chunkMesh.indices.push_back(Render::FACE_INDICES[0][i] + baseIndex);
      }
      baseIndex += 4;
    }
    if (z == 0 || chunkData.voxels[getIdx(x, y, z - 1)].type == 0) {
      // Back face
      for (int i = 0; i < 6; ++i) {
        if (i < 4) {
          Render::Vertex vert = Render::FACE_VERTICES[1][i];
          vert.position += glm::vec3(fx, fy, fz);
          chunkMesh.vertices.push_back(vert);
        }
        chunkMesh.indices.push_back(Render::FACE_INDICES[1][i] + baseIndex);
      }
      baseIndex += 4;
    }
    if (x == CHUNK_WIDTH - 1 ||
        chunkData.voxels[getIdx(x + 1, y, z)].type == 0) {
      // Right face
      for (int i = 0; i < 6; ++i) {
        if (i < 4) {
          Render::Vertex vert = Render::FACE_VERTICES[2][i];
          vert.position += glm::vec3(fx, fy, fz);
          chunkMesh.vertices.push_back(vert);
        }
        chunkMesh.indices.push_back(Render::FACE_INDICES[2][i] + baseIndex);
      }
      baseIndex += 4;
    }
    if (x == 0 || chunkData.voxels[getIdx(x - 1, y, z)].type == 0) {
      // Left face
      for (int i = 0; i < 6; ++i) {
        if (i < 4) {
          Render::Vertex vert = Render::FACE_VERTICES[3][i];
          vert.position += glm::vec3(fx, fy, fz);
          chunkMesh.vertices.push_back(vert);
        }
        chunkMesh.indices.push_back(Render::FACE_INDICES[3][i] + baseIndex);
      }
      baseIndex += 4;
    }
    if (y == CHUNK_WIDTH - 1 ||
        chunkData.voxels[getIdx(x, y + 1, z)].type == 0) {
      // Top face
      for (int i = 0; i < 6; ++i) {
        if (i < 4) {
          Render::Vertex vert = Render::FACE_VERTICES[4][i];
          vert.position += glm::vec3(fx, fy, fz);
          chunkMesh.vertices.push_back(vert);
        }
        chunkMesh.indices.push_back(Render::FACE_INDICES[4][i] + baseIndex);
      }
      baseIndex += 4;
    }
    if (y == 0 || chunkData.voxels[getIdx(x, y - 1, z)].type == 0) {
      // Bottom face
      for (int i = 0; i < 6; ++i) {
        if (i < 4) {
          Render::Vertex vert = Render::FACE_VERTICES[5][i];
          vert.position += glm::vec3(fx, fy, fz);
          chunkMesh.vertices.push_back(vert);
        }
        chunkMesh.indices.push_back(Render::FACE_INDICES[5][i] + baseIndex);
      }
      baseIndex += 4;
    }

    // Render::Vertex vertices[24] = {
    //   //Front face
    //   {{fx,     fy,     fz + 1}, Render::FACE_NORMALS[0]},
    //   {{fx + 1, fy,     fz + 1}, Render::FACE_NORMALS[0]},
    //   {{fx + 1, fy + 1, fz + 1}, Render::FACE_NORMALS[0]},
    //   {{fx,     fy + 1, fz + 1}, Render::FACE_NORMALS[0]},
    //   //Back face
    //   {{fx + 1, fy,     fz},     Render::FACE_NORMALS[1]},
    //   {{fx,     fy,     fz},     Render::FACE_NORMALS[1]},
    //   {{fx,     fy + 1, fz},     Render::FACE_NORMALS[1]},
    //   {{fx + 1, fy + 1, fz},     Render::FACE_NORMALS[1]},
    //   //Right face
    //   {{fx + 1, fy,     fz + 1}, Render::FACE_NORMALS[2]},
    //   {{fx + 1, fy,     fz},     Render::FACE_NORMALS[2]},
    //   {{fx + 1, fy + 1, fz},     Render::FACE_NORMALS[2]},
    //   {{fx + 1, fy + 1, fz + 1}, Render::FACE_NORMALS[2]},
    //   //Left face
    //   {{fx,     fy,     fz},     Render::FACE_NORMALS[3]},
    //   {{fx,     fy,     fz + 1}, Render::FACE_NORMALS[3]},
    //   {{fx,     fy + 1, fz + 1}, Render::FACE_NORMALS[3]},
    //   {{fx,     fy + 1, fz},     Render::FACE_NORMALS[3]},
    //   //Top face
    //   {{fx,     fy + 1, fz + 1}, Render::FACE_NORMALS[4]},
    //   {{fx + 1, fy + 1, fz + 1}, Render::FACE_NORMALS[4]},
    //   {{fx + 1, fy + 1, fz},     Render::FACE_NORMALS[4]},
    //   {{fx,     fy + 1, fz},     Render::FACE_NORMALS[4]},
    //   //Bottom face
    //   {{fx,     fy,     fz},     Render::FACE_NORMALS[5]},
    //   {{fx + 1, fy,     fz},     Render::FACE_NORMALS[5]},
    //   {{fx + 1, fy,     fz + 1}, Render::FACE_NORMALS[5]},
    //   {{fx,     fy,     fz + 1}, Render::FACE_NORMALS[5]},
    // };
    // chunkMesh.vertices.insert(chunkMesh.vertices.end(), std::begin(vertices),
    // std::end(vertices)); unsigned int indices[36] = {
    //   //Front face
    //   baseIndex, baseIndex + 1, baseIndex + 2,
    //   baseIndex, baseIndex + 2, baseIndex + 3,
    //   //Back face
    //   baseIndex + 4, baseIndex + 6, baseIndex + 5,
    //   baseIndex + 4, baseIndex + 7, baseIndex + 6,
    //   //Right face
    //   baseIndex + 8, baseIndex + 9, baseIndex + 10,
    //   baseIndex + 8, baseIndex + 10, baseIndex + 11,
    //   //Left face
    //   baseIndex + 12, baseIndex + 14, baseIndex + 13,
    //   baseIndex + 12, baseIndex + 15, baseIndex + 14,
    //   //Top face
    //   baseIndex + 16, baseIndex + 17, baseIndex + 18,
    //   baseIndex + 16, baseIndex + 18, baseIndex + 19,
    //   //Bottom face
    //   baseIndex + 20, baseIndex + 22, baseIndex + 21,
    //   baseIndex + 20, baseIndex + 23, baseIndex + 22,
    // };
    // chunkMesh.indices.insert(chunkMesh.indices.end(), std::begin(indices),
    // std::end(indices));
  }
  chunkMesh.setupMesh();
  SDL_Log("Chunk mesh built in %llu ms, %llu verts", SDL_GetTicks() - startTime,
          chunkMesh.vertices.size());
}

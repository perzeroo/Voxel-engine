#include "SDL3/SDL_log.h"
#include "SDL3/SDL_timer.h"
#include "core/Common.hpp"
#include "engine/Render.hpp"
#include <engine/chunk/ChunkMeshBuilder.hpp>

void Engine::Chunk::ChunkMeshBuilder::on_construct(entt::registry &registry, entt::entity entity) {
  Engine::Chunk::ChunkMeshBuilder::on_chunk_data_change(registry, entity);
}

void Engine::Chunk::ChunkMeshBuilder::on_chunk_data_change(entt::registry &registry, entt::entity entity) {
  auto &chunkData = registry.get<ChunkData>(entity);
  if (!registry.all_of<ChunkMesh>(entity)) {
    registry.emplace<ChunkMesh>(entity);
  }
  auto &chunkMesh = registry.get<ChunkMesh>(entity);
  build(chunkData, chunkMesh);
}

void Engine::Chunk::ChunkMeshBuilder::build(ChunkData &chunkData, ChunkMesh &chunkMesh) {
  SDL_Log("Building chunk mesh... Time: %u ticks", SDL_GetTicks());

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

    Render::Vertex vertices[24] = {
      //Front face
      {{fx,     fy,     fz + 1}, Render::FACE_NORMALS[0]},
      {{fx + 1, fy,     fz + 1}, Render::FACE_NORMALS[0]},
      {{fx + 1, fy + 1, fz + 1}, Render::FACE_NORMALS[0]},
      {{fx,     fy + 1, fz + 1}, Render::FACE_NORMALS[0]},
      //Back face
      {{fx + 1, fy,     fz},     Render::FACE_NORMALS[1]},
      {{fx,     fy,     fz},     Render::FACE_NORMALS[1]},
      {{fx,     fy + 1, fz},     Render::FACE_NORMALS[1]},
      {{fx + 1, fy + 1, fz},     Render::FACE_NORMALS[1]},
      //Right face
      {{fx + 1, fy,     fz + 1}, Render::FACE_NORMALS[2]},
      {{fx + 1, fy,     fz},     Render::FACE_NORMALS[2]},
      {{fx + 1, fy + 1, fz},     Render::FACE_NORMALS[2]},
      {{fx + 1, fy + 1, fz + 1}, Render::FACE_NORMALS[2]},
      //Left face
      {{fx,     fy,     fz},     Render::FACE_NORMALS[3]},
      {{fx,     fy,     fz + 1}, Render::FACE_NORMALS[3]},
      {{fx,     fy + 1, fz + 1}, Render::FACE_NORMALS[3]},
      {{fx,     fy + 1, fz},     Render::FACE_NORMALS[3]},
      //Top face
      {{fx,     fy + 1, fz + 1}, Render::FACE_NORMALS[4]},
      {{fx + 1, fy + 1, fz + 1}, Render::FACE_NORMALS[4]},
      {{fx + 1, fy + 1, fz},     Render::FACE_NORMALS[4]},
      {{fx,     fy + 1, fz},     Render::FACE_NORMALS[4]},
      //Bottom face
      {{fx,     fy,     fz},     Render::FACE_NORMALS[5]},
      {{fx + 1, fy,     fz},     Render::FACE_NORMALS[5]},
      {{fx + 1, fy,     fz + 1}, Render::FACE_NORMALS[5]},
      {{fx,     fy,     fz + 1}, Render::FACE_NORMALS[5]},
    };
    unsigned int baseIndex = static_cast<unsigned int>(chunkMesh.vertices.size());
    chunkMesh.vertices.insert(chunkMesh.vertices.end(), std::begin(vertices), std::end(vertices));
    unsigned int indices[36] = {
      //Front face
      baseIndex, baseIndex + 1, baseIndex + 2,
      baseIndex, baseIndex + 2, baseIndex + 3,
      //Back face
      baseIndex + 4, baseIndex + 6, baseIndex + 5,
      baseIndex + 4, baseIndex + 7, baseIndex + 6,
      //Right face
      baseIndex + 8, baseIndex + 9, baseIndex + 10,
      baseIndex + 8, baseIndex + 10, baseIndex + 11,
      //Left face
      baseIndex + 12, baseIndex + 14, baseIndex + 13,
      baseIndex + 12, baseIndex + 15, baseIndex + 14,
      //Top face
      baseIndex + 16, baseIndex + 17, baseIndex + 18,
      baseIndex + 16, baseIndex + 18, baseIndex + 19,
      //Bottom face
      baseIndex + 20, baseIndex + 22, baseIndex + 21,
      baseIndex + 20, baseIndex + 23, baseIndex + 22,
    };
    chunkMesh.indices.insert(chunkMesh.indices.end(), std::begin(indices), std::end(indices));
  }
  SDL_Log("Chunk mesh built... Time: %u ticks", SDL_GetTicks());
}

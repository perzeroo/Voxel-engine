#include "engine/Engine.hpp"
#include "engine/chunk/ChunkMesh.hpp"
#include <engine/chunk/ChunkData.hpp>
#include <engine/chunk/ChunkMeshBuilder.hpp>
#include <memory>

void Engine::Chunk::ChunkData::on_update(entt::registry &registry,
                                         entt::entity entity) {
  if (registry.all_of<ChunkMeshBuilder>(entity)) {
    // ChunkMeshBuilder::on_chunk_data_change(registry, entity);
    // registry.emplace<Engine::Dirty>(entity);
  }
}

bool Engine::Chunk::ChunkData::buildChunkMesh(
    const ChunkNeighborhood &neighborhood,
    std::shared_ptr<ChunkMeshData> chunkMesh) {

  if (!chunkMesh) {
    return false;
  }
  chunkMesh->vertices.reserve(CHUNK_SIZE * 12);
  chunkMesh->indices.reserve(CHUNK_SIZE * 18);
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

    bool nFront, nBack, nRight, nLeft, nTop, nBottom;
    nFront = false;
    nBack = false;
    nRight = false;
    nLeft = false;
    nTop = false;
    nBottom = false;

    if (neighborhood.pz) {
      nFront = neighborhood.pz->voxels[getIdx(x, y, 0)].type != 0;
    }
    if (neighborhood.nz) {
      nBack = neighborhood.nz->voxels[getIdx(x, y, CHUNK_WIDTH - 1)].type != 0;
    }
    if (neighborhood.px) {
      nRight = neighborhood.px->voxels[getIdx(0, y, z)].type != 0;
    }
    if (neighborhood.nx) {
      nLeft = neighborhood.nx->voxels[getIdx(CHUNK_WIDTH - 1, y, z)].type != 0;
    }
    if (neighborhood.py) {
      nTop = neighborhood.py->voxels[getIdx(x, 0, z)].type != 0;
    }
    if (neighborhood.ny) {
      nBottom =
          neighborhood.ny->voxels[getIdx(x, CHUNK_WIDTH - 1, z)].type != 0;
    }

    if ((z == CHUNK_WIDTH - 1 && !nFront) || getType(x, y, z + 1) == 0) {
      // Front face
      chunkMesh->addFaceFromIndex(0, fx, fy, fz, voxels[i]);
    }
    if ((z == 0 && !nBack) || getType(x, y, z - 1) == 0) {
      // Back face
      chunkMesh->addFaceFromIndex(1, fx, fy, fz, voxels[i]);
    }
    if ((x == CHUNK_WIDTH - 1 && !nRight) || getType(x + 1, y, z) == 0) {
      // Right face
      chunkMesh->addFaceFromIndex(2, fx, fy, fz, voxels[i]);
    }
    if ((x == 0 && !nLeft) || getType(x - 1, y, z) == 0) {
      // Left face
      chunkMesh->addFaceFromIndex(3, fx, fy, fz, voxels[i]);
    }
    if ((y == CHUNK_WIDTH - 1 && !nTop) || getType(x, y + 1, z) == 0) {
      // Top face
      chunkMesh->addFaceFromIndex(4, fx, fy, fz, voxels[i]);
    }
    if ((y == 0 && !nBottom) || getType(x, y - 1, z) == 0) {
      // Bottom face
      chunkMesh->addFaceFromIndex(5, fx, fy, fz, voxels[i]);
    }
  }
  return true;
}

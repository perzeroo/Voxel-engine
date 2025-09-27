#pragma once

#include "ChunkData.hpp"
#include "ChunkMesh.hpp"
namespace Engine::Chunk {
struct ChunkMeshBuilder {
  void build(ChunkData &chunkData, ChunkMesh &chunkMesh); 
};
}

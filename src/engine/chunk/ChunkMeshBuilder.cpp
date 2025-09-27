#include "core/Common.hpp"
#include <engine/chunk/ChunkMeshBuilder.hpp>

void Engine::Chunk::ChunkMeshBuilder::build(ChunkData &chunkData, ChunkMesh &chunkMesh) {
  for (int i = 0; i < CHUNK_SIZE; i++) {
    int z = i / (CHUNK_WIDTH * CHUNK_WIDTH);
    int y = (i % (CHUNK_WIDTH * CHUNK_WIDTH)) / CHUNK_WIDTH;
    int x = i % CHUNK_WIDTH;

    
  }
}

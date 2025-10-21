#include "engine/world/WorldGenerator.hpp"
#include "SDL3/SDL_log.h"
#include "core/Common.hpp"
#include "engine/chunk/ChunkData.hpp"
#include "engine/voxel/Voxel.hpp"

Engine::World::WorldGenerator::WorldGenerator() {
  noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  noise.SetFrequency(0.01f);
}

void Engine::World::WorldGenerator::generateChunk(
    int chunkX, int chunkY, int chunkZ, Engine::Chunk::ChunkData &chunkData) {
  for (int x = 0; x < CHUNK_WIDTH; x++) {
    for (int z = 0; z < CHUNK_WIDTH; z++) {
      int worldX = chunkX * CHUNK_WIDTH + x;
      int worldZ = chunkZ * CHUNK_WIDTH + z;

      float heightValue = noise.GetNoise((float)worldX, (float)worldZ);
      int height = static_cast<int>((heightValue + 1.0f) * 0.5f *
                                    31); // Scale to [0, 32]

      for (int y = 0; y < CHUNK_WIDTH; y++) {
        int worldY = chunkY * CHUNK_WIDTH + y;
        int index = Chunk::getIdx(x, y, z);
        if (worldY < height) {
          chunkData.voxels[index].type = Voxel::VoxelType::Dirt;
        } else if (worldY == height) {
          chunkData.voxels[index].type = Voxel::VoxelType::Grass;
        } else {
          chunkData.voxels[index].type = Voxel::VoxelType::Air;
        }
      }
    }
  }
}

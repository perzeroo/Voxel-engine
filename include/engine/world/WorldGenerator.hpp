#pragma once
#include "engine/chunk/ChunkData.hpp"
#include <thirdparty/FastNoiseLite/FastNoiseLite.hpp>

namespace Engine::World {
class WorldGenerator {
public:
  WorldGenerator();
  ~WorldGenerator() = default;

  void generateChunk(int chunkX, int chunkY, int chunkZ,
                     Engine::Chunk::ChunkData &chunkData);

private:
  FastNoiseLite noise;
};
} // namespace Engine::World

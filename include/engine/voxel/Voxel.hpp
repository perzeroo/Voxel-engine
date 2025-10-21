#pragma once

#include <cstdint>

namespace Engine::Voxel {
enum VoxelType : uint8_t {
  Air = 0,
  Dirt = 1,
  Grass = 2,
};
typedef struct Voxel {
  VoxelType type;
} Voxel;

}

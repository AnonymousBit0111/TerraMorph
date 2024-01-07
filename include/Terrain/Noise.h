#pragma once

#include "FastNoiseLite.h"
#include "glm/glm.hpp"
#include <vector>
namespace TerraMorph {
namespace Terrain {
class Noise {

public:
  static std::vector<std::vector<float>>
  generateNoiseMap(glm::vec2 size, float scale, int octaves, float persistance,
                   float lacunarity);
};
} // namespace Terrain
} // namespace TerraMorph
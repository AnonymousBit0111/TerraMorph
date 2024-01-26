#include "Terrain/Noise.h"
#include "Core/Logging.h"
#include "FastNoiseLite.h"
#include <cassert>
#include <string>
#include <vector>
using namespace TerraMorph::Terrain;

std::vector<std::vector<float>>
Noise::generateNoiseMap(glm::vec2 size, float scale, int octaves,
                        float persistance, float lacunarity) {

  if (scale <= 0.0f) {
    scale = 0.00001f;
  }

  std::vector<std::vector<float>> noiseMap;

  noiseMap.resize(size.x);

  FastNoiseLite noise;
  noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  float maxHeight = 0;
  for (int x = 0; x < size.x; x++) {
    noiseMap[x] = std::vector<float>();
    noiseMap[x].resize(size.y);
    for (int y = 0; y < size.y; y++) {

      float noiseHeight = 0.0f;

      for (int oct = 0; oct < octaves; oct++) {
        float frequency = pow(lacunarity, oct);
        float amplitude = pow(persistance, oct);

        float sampleX = (x / scale) * frequency;
        float sampleY = (y / scale) * frequency;

        noiseHeight += noise.GetNoise(sampleX, sampleY) * amplitude;
      }
      noiseMap[x][y] = noiseHeight;
    }
  }

  // to get all the values < 1, we can divide every value by the maximum height

  for (auto &&i : noiseMap) {
    for (auto &&height : i) {
      if (std::abs(height) > maxHeight) {
        maxHeight = std::abs(height);
      }
    }
  }

  for (auto &&i : noiseMap) {
    for (auto &&height : i) {

      if (maxHeight != 0)
        height /= maxHeight;

      // Core::Logging::log(std::to_string(height));
      assert(abs(height) <= 1.0f);
    }
  }

  return noiseMap;
}
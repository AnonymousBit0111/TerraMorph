#pragma once
#include "glm/glm.hpp"
namespace TerraMorph {

namespace Core {

struct PosColourVertex {
  float x, y, z, r, g, b, a;
};

struct PushConstant {
  glm::vec4 data;
  glm::mat4 viewproj;
};
} // namespace Core
} // namespace TerraMorph
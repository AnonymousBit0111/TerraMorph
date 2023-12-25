#pragma once
#include "glm/glm.hpp"
namespace TerraMorph {

namespace Core {

struct PosColourVertex {
  glm::vec3 pos;
  glm::vec4 colour;
};

struct PushConstant {
  glm::vec4 data;
  glm::mat4 viewproj;
};
} // namespace Core
} // namespace TerraMorph
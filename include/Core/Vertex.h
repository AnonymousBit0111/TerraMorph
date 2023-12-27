#pragma once
#include "glm/glm.hpp"
namespace TerraMorph {

namespace Core {

struct PosColourVertex {
  glm::vec3 pos;
  glm::vec4 colour;
};

struct PushConstant {
  glm::mat4 rotationData;
  glm::mat4 viewproj;
};
} // namespace Core
} // namespace TerraMorph
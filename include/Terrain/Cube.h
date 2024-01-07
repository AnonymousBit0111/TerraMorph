#pragma once
#include "Graphics/InstanceBuffer.h"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"
namespace TerraMorph {
namespace Terrain {
class Cube {

private:
  glm::vec3 m_pos;
  Graphics::InstanceData *p_instanceData = nullptr;

public:
  Cube(glm::vec3 position, Graphics::InstanceData *instanceData);
  void setPosition(glm::vec3 position);
  glm::vec3 getPosition() const { return m_pos; }

  void setInstanceData(Graphics::InstanceData *instancedata) {
    this->p_instanceData = instancedata;
  }
  Graphics::InstanceData *getinstanceData() const { return p_instanceData; }
  ~Cube();
};
} // namespace Terrain
} // namespace TerraMorph
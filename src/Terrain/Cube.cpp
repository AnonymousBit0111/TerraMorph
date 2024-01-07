#include "Terrain/Cube.h"
#include "Graphics/InstanceBuffer.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"

using TerraMorph::Graphics::InstanceData;

using namespace TerraMorph::Terrain;

Cube::Cube(glm::vec3 position, InstanceData *instancedata)
    : m_pos(position), p_instanceData(instancedata) {
  if (p_instanceData != nullptr) {
    p_instanceData->model = glm::translate(p_instanceData->model, m_pos);

    float red = m_pos.y / 100;

    if (red > 0) {
      p_instanceData->colour =
          glm::vec4(0.0f, 0.0f, red, 1.0f);

    } else {
      p_instanceData->colour =
          glm::vec4(red*-1, 0.0f, 0.0f, 1.0f);
    }

    // vulkan flips y values
  }
}

void Cube::setPosition(glm::vec3 position) {
  m_pos = position;
  if (p_instanceData != nullptr) {
    p_instanceData->model = glm::translate(glm::mat4(1.0f), m_pos);
  }
}

Cube::~Cube() {}
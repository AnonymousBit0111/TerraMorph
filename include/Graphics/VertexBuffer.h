#pragma once

#include "glm/fwd.hpp"
#include "vulkan/vulkan.hpp"
#include "Core/Vertex.h"
namespace TerraMorph {
namespace Graphics {

class VertexBuffer {

private:
  glm::uint32_t m_size;

  vk::Buffer m_handle;
  vk::DeviceMemory m_memory;
  void *m_data;

public:
  VertexBuffer(glm::uint32_t size);
  vk::Buffer getHandle() const { return m_handle; }
  void update(const std::vector<Core::PosColourVertex> &vertices);
  ~VertexBuffer();
};



}; // namespace Graphics
} // namespace TerraMorph
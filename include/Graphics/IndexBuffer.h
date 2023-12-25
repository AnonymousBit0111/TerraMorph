#pragma once

#include "Core/Vertex.h"
#include "glm/fwd.hpp"
#include "vulkan/vulkan.hpp"
namespace TerraMorph {
namespace Graphics {

class IndexBuffer {

private:
  glm::uint32_t m_size;

  vk::Buffer m_handle;
  vk::DeviceMemory m_memory;
  void *m_data;

public:
  IndexBuffer(glm::uint32_t size);
  vk::Buffer getHandle() const { return m_handle; }
  void update(const std::vector<glm::uint32_t> &indices);
  ~IndexBuffer();
};

}; // namespace Graphics
} // namespace TerraMorph
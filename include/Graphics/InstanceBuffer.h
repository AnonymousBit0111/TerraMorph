#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_handles.hpp"
#include <vector>
namespace TerraMorph {
namespace Graphics {

struct InstanceData {
  glm::mat4 model;
  glm::vec4 colour;
};
class InstanceBuffer {

private:
  glm::uint32_t m_size;

  vk::Buffer m_handle;
  vk::DeviceMemory m_memory;
  void *m_data;

public:
  InstanceBuffer(glm::uint32_t size);

  void update(std::vector<InstanceData> &matrices);
  void resize(glm::uint32_t size);
  vk::Buffer getHandle() const { return m_handle; }
  ~InstanceBuffer();
};

} // namespace Graphics
} // namespace TerraMorph
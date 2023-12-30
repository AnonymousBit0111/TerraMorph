#pragma once

#include "glm/fwd.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_handles.hpp"
#include <vector>
namespace TerraMorph {
namespace Graphics {
class InstanceBuffer {

private:
  glm::uint32_t m_size;

  vk::Buffer m_handle;
  vk::DeviceMemory m_memory;
  void *m_data;

public:
  InstanceBuffer(glm::uint32_t size);

  void update(std::vector<glm::mat4> &matrices);
  void resize(glm::uint32_t size);
  vk::Buffer getHandle()const{return m_handle;}
  ~InstanceBuffer();
};


} // namespace Graphics
} // namespace TerraMorph
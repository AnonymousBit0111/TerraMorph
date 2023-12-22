#pragma once

#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_handles.hpp"
namespace TerraMorph {

namespace Graphics {

class RenderPass {
private:
  vk::RenderPass m_handle;

public:
  RenderPass(vk::Format imageFormat);
  vk::RenderPass getHandle() const { return m_handle; }
  ~RenderPass();
};
} // namespace Graphics

} // namespace TerraMorph
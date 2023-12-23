#pragma once

#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"
#include <vector>
namespace TerraMorph {
namespace Graphics {
class PipelineLayout {
private:
  vk::PipelineLayout m_handle;
  vk::PipelineLayoutCreateInfo m_createInfo;

  std::vector<vk::PushConstantRange> m_pushConstantRanges;
  std::vector<vk::DescriptorSetLayout> m_descriptorSetLayouts;

public:
  PipelineLayout();
  void recreate();
  void
  addDescriptorSetLayout(const vk::DescriptorSetLayout &descriptorSetLayout);
  void addPushConstantRange(const vk::PushConstantRange &pushConstantRange);
  vk::PipelineLayout getHandle() const { return m_handle; }
  ~PipelineLayout();
};

} // namespace Graphics
} // namespace TerraMorph
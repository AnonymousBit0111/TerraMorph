#pragma once
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_structs.hpp"
namespace TerraMorph {

namespace Graphics {
class Pipeline {

private:
  vk::Pipeline m_handle;
  std::array<vk::VertexInputBindingDescription,2> m_bindingDescs;
  std::vector<vk::VertexInputAttributeDescription >m_attributeDescriptions;

public:
  Pipeline(const std::string &fsPath, const std::string &vsPath,
           vk::RenderPass rp, vk::PipelineLayout layout,
           vk::Extent2D viewportSize);
  vk::Pipeline getHandle()const {return m_handle;}

  ~Pipeline();
};

} // namespace Graphics
} // namespace TerraMorph
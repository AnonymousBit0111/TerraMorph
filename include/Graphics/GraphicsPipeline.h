#pragma once
#include "vulkan/vulkan.hpp"
namespace TerraMorph {

namespace Graphics {
class Pipeline {

private:
  vk::Pipeline m_handle;

public:
  Pipeline();

  ~Pipeline();
};

} // namespace Graphics
} // namespace TerraMorph
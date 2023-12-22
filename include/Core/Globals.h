#pragma once

#include "Graphics/VkContext.h"
namespace TerraMorph {
namespace Core {
// NOTE : Consider moving this into the graphics folder and namespace considering it
// relates solely to vulkan
extern std::shared_ptr<Graphics::VKContext> g_vkContext;
} // namespace Core
} // namespace TerraMorph
#pragma once

#include "Graphics/RenderPass.h"
#include "Graphics/Swapchain.h"
#include <SDL_video.h>
#include <memory>
namespace TerraMorph {
namespace Graphics {
class Renderer {

private:
  std::unique_ptr<Swapchain> m_swapChain;
  std::unique_ptr<RenderPass> m_renderPass;

public:
  Renderer(SDL_Window *win);
  ~Renderer();
};
} // namespace Graphics
} // namespace TerraMorph
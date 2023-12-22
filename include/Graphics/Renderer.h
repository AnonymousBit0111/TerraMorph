#pragma once

#include "Graphics/Swapchain.h"
#include <SDL_video.h>
namespace TerraMorph {
namespace Graphics {
class Renderer {

private:

std::shared_ptr<Swapchain> m_swapChain;




public:
  Renderer(SDL_Window *win);
  ~Renderer();
};
} // namespace Graphics
} // namespace TerraMorph
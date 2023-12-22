#include "Graphics/Renderer.h"
#include "Graphics/Swapchain.h"
#include <memory>

using namespace TerraMorph::Graphics;

Renderer::Renderer(SDL_Window *window) {
  m_swapChain = std::make_shared<Swapchain>(window);
}

Renderer::~Renderer() {}
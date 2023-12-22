#include "Graphics/Renderer.h"
#include "Graphics/RenderPass.h"
#include "Graphics/Swapchain.h"
#include <memory>

using namespace TerraMorph::Graphics;

Renderer::Renderer(SDL_Window *window) {
  m_swapChain = std::make_unique<Swapchain>(window);
  m_renderPass = std::make_unique<RenderPass>(m_swapChain->getImageFormat());
  m_swapChain->createFrameBuffers(m_renderPass->getHandle());
}

Renderer::~Renderer() {}
#pragma once

#include "Graphics/GraphicsPipeline.h"
#include "Graphics/PipelineLayout.h"
#include "Graphics/RenderPass.h"
#include "Graphics/Swapchain.h"
#include "vulkan/vulkan_handles.hpp"
#include <SDL_video.h>
#include <memory>
namespace TerraMorph {
namespace Graphics {
class Renderer {

private:
  std::unique_ptr<Swapchain> m_swapChain;
  std::unique_ptr<RenderPass> m_renderPass;
  std::unique_ptr<PipelineLayout> m_pipelineLayout;
  std::unique_ptr<Pipeline> m_pipeline;

  // TODO , render more than 1 frame ahead
  vk::Semaphore m_renderFinished;
  vk::Semaphore m_imageAvailable;
  vk::Fence m_frameInflight;

public:
  Renderer(SDL_Window *win);
  void beginFrame();
  ~Renderer();
};
} // namespace Graphics
} // namespace TerraMorph
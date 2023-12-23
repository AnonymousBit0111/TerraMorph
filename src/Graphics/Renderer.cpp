#include "Graphics/Renderer.h"
#include "Core/Globals.h"
#include "Core/Vertex.h"
#include "Graphics/GraphicsPipeline.h"
#include "Graphics/PipelineLayout.h"
#include "Graphics/RenderPass.h"
#include "Graphics/Swapchain.h"
#include <memory>
using namespace TerraMorph::Graphics;
using namespace TerraMorph::Core;

Renderer::Renderer(SDL_Window *window) {
  m_swapChain = std::make_unique<Swapchain>(window);
  m_renderPass = std::make_unique<RenderPass>(m_swapChain->getImageFormat());
  m_swapChain->createFrameBuffers(m_renderPass->getHandle());
  m_pipelineLayout = std::make_unique<PipelineLayout>();
  vk::PushConstantRange pushConstantRange{

  };
  pushConstantRange.offset = 0;

  pushConstantRange.size = sizeof(Core::PushConstant);
  pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;

  m_pipelineLayout->addPushConstantRange(pushConstantRange);

  m_pipeline = std::make_unique<Pipeline>(
      "res/simplefrag.spv", "res/simplevert.spv", m_renderPass->getHandle(),
      m_pipelineLayout->getHandle(), m_swapChain->getExtent());

  vk::SemaphoreCreateInfo sephInfo{};
  vk::FenceCreateInfo fenceInfo{};
  fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

  vk::resultCheck(g_vkContext->device.createSemaphore(&sephInfo, nullptr,
                                                      &m_imageAvailable),
                  "failed to create imageSemaphore");
  vk::resultCheck(g_vkContext->device.createSemaphore(&sephInfo, nullptr,
                                                      &m_renderFinished),
                  "Failed to create renderfinished semaphore");

  vk::resultCheck(
      g_vkContext->device.createFence(&fenceInfo, nullptr, &m_frameInflight),
      "failed to create inFlight fence");
}

Renderer::~Renderer() {

  // TODO , make sure there are no future mutex lock issues
  g_vkContext->device.destroySemaphore(m_imageAvailable);
  g_vkContext->device.destroySemaphore(m_renderFinished);
  g_vkContext->device.destroyFence(m_frameInflight);
}
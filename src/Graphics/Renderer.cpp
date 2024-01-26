#include "Graphics/Renderer.h"
#include "Core/Globals.h"
#include "Core/Vertex.h"
#include "Graphics/Camera.h"
#include "Graphics/GraphicsPipeline.h"
#include "Graphics/InstanceBuffer.h"
#include "Graphics/PipelineLayout.h"
#include "Graphics/RenderPass.h"
#include "Graphics/Swapchain.h"
#include "Graphics/VertexBuffer.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"
#include <array>
#include <cassert>
#include <cstdint>
#include <memory>

using namespace TerraMorph::Graphics;
using namespace TerraMorph::Core;

void Renderer::initImGui() {

  ImGui::CreateContext();

  std::vector<vk::DescriptorPoolSize> poolSizes = {
      {vk::DescriptorType::eSampler, 1000},
      {vk::DescriptorType::eCombinedImageSampler, 1000},
      {vk::DescriptorType::eSampledImage, 1000},
      {vk::DescriptorType::eStorageImage, 1000},
      {vk::DescriptorType::eUniformTexelBuffer, 1000},
      {vk::DescriptorType::eStorageTexelBuffer, 1000},
      {vk::DescriptorType::eUniformBuffer, 1000},
      {vk::DescriptorType::eStorageBuffer, 1000},
      {vk::DescriptorType::eUniformBufferDynamic, 1000},
      {vk::DescriptorType::eStorageBufferDynamic, 1000},
      {vk::DescriptorType::eInputAttachment, 1000}};

  // Create the descriptor pool
  vk::DescriptorPoolCreateInfo poolInfo = {
      {vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet}, // Flags
      1000,                                                   // Max sets
      static_cast<uint32_t>(poolSizes.size()),                // Pool size count
      poolSizes.data()                                        // Pool sizes
  };

  imGuiDescriptorPool = g_vkContext->device.createDescriptorPool(poolInfo);

  ImGui_ImplVulkan_InitInfo initInfo{};
  initInfo.Allocator = nullptr;
  initInfo.Instance = g_vkContext->instance;
  initInfo.ImageCount = m_swapChain->getImageCount();
  initInfo.MinImageCount = 2;
  initInfo.Queue = g_vkContext->graphicsQueue;

  initInfo.QueueFamily = g_vkContext->queueFamilyIndices["Graphics"].value();
  initInfo.Device = g_vkContext->device;
  initInfo.PhysicalDevice = g_vkContext->physicalDevice;
  initInfo.CheckVkResultFn = nullptr;
  initInfo.DescriptorPool = imGuiDescriptorPool;
  initInfo.PipelineCache = {};

  ImGui_ImplSDL2_InitForVulkan(p_window);
  ImGui_ImplVulkan_Init(&initInfo, m_renderPass->getHandle());

  ImGui::StyleColorsDark();
}

Renderer::Renderer(SDL_Window *window, Camera *cam, int instancecount)
    : p_window(window), camera(cam), m_maxInstanceCount(instancecount) {
  m_swapChain = std::make_unique<Swapchain>(window);
  m_renderPass = std::make_unique<RenderPass>(m_swapChain->getImageFormat());
  m_swapChain->createFrameBuffers(m_renderPass->getHandle());
  m_pipelineLayout = std::make_unique<PipelineLayout>();

  m_instanceBuffer = std::make_unique<InstanceBuffer>(m_maxInstanceCount *
                                                      sizeof(InstanceData));
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

  vk::CommandPoolCreateInfo poolInfo{};
  poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
  poolInfo.queueFamilyIndex =
      g_vkContext->queueFamilyIndices["Graphics"].value();

  assert(g_vkContext->device.createCommandPool(
             &poolInfo, nullptr, &m_commandPool) == vk::Result::eSuccess);

  vk::CommandBufferAllocateInfo info{};
  info.commandPool = m_commandPool;
  info.level = vk::CommandBufferLevel::ePrimary;
  info.commandBufferCount = 1;

  assert(g_vkContext->device.allocateCommandBuffers(&info, &m_commandBuffer) ==
         vk::Result::eSuccess);

  initImGui();

  glm::vec4 cubeColour = glm::vec4(0.5f);

  cubeVertices = {// Front face
                  {
                      {glm::vec3{-1.0, -1.0, 1.0}, cubeColour}, // bottom-left
                      {glm::vec3{1.0, -1.0, 1.0}, cubeColour},  // bottom-right
                      {glm::vec3{1.0, 1.0, 1.0}, cubeColour},   // top-right
                      {glm::vec3{-1.0, 1.0, 1.0}, cubeColour},  // top-left

                      // Back face
                      {glm::vec3{1.0, -1.0, -1.0}, cubeColour},  // bottom-left
                      {glm::vec3{-1.0, -1.0, -1.0}, cubeColour}, // bottom-right
                      {glm::vec3{-1.0, 1.0, -1.0}, cubeColour},  // top-right
                      {glm::vec3{1.0, 1.0, -1.0}, cubeColour},   // top-left

                      // Left face
                      {glm::vec3{-1.0, -1.0, -1.0}, cubeColour}, // bottom-left
                      {glm::vec3{-1.0, -1.0, 1.0}, cubeColour},  // bottom-right
                      {glm::vec3{-1.0, 1.0, 1.0}, cubeColour},   // top-right
                      {glm::vec3{-1.0, 1.0, -1.0}, cubeColour},  // top-left

                      // Right face
                      {glm::vec3{1.0, -1.0, 1.0}, cubeColour},  // bottom-left
                      {glm::vec3{1.0, -1.0, -1.0}, cubeColour}, // bottom-right
                      {glm::vec3{1.0, 1.0, -1.0}, cubeColour},  // top-right
                      {glm::vec3{1.0, 1.0, 1.0}, cubeColour},   // top-left

                      // Top face
                      {glm::vec3{-1.0, 1.0, 1.0}, cubeColour},  // bottom-left
                      {glm::vec3{1.0, 1.0, 1.0}, cubeColour},   // bottom-right
                      {glm::vec3{1.0, 1.0, -1.0}, cubeColour},  // top-right
                      {glm::vec3{-1.0, 1.0, -1.0}, cubeColour}, // top-left

                      // Bottom face
                      {glm::vec3{-1.0, -1.0, -1.0}, cubeColour}, // bottom-left
                      {glm::vec3{1.0, -1.0, -1.0}, cubeColour},  // bottom-right
                      {glm::vec3{1.0, -1.0, 1.0}, cubeColour},   // top-right
                      {glm::vec3{-1.0, -1.0, 1.0}, cubeColour}   // top-left
                  }};

  m_vertexBuffer = std::make_unique<VertexBuffer>(sizeof(PosColourVertex) *
                                                  cubeVertices.size());
  m_vertexBuffer->update(cubeVertices);

  cubeIndices = {
      0,  1,  2,  2,  3,  0,  // Front face
      4,  5,  6,  6,  7,  4,  // Back face
      8,  9,  10, 10, 11, 8,  // Left face
      12, 13, 14, 14, 15, 12, // Right face
      16, 17, 18, 18, 19, 16, // Top face
      20, 21, 22, 22, 23, 20  // Bottom face
  };
  m_indexBuffer =
      std::make_unique<IndexBuffer>(cubeIndices.size() * sizeof(glm::uint32_t));

  m_indexBuffer->update(cubeIndices);

}

void Renderer::beginFrame() {
  // NOTE, if frame skipping is ever needed , use the timeout parameter
  auto result = g_vkContext->device.waitForFences(1, &m_frameInflight,
                                                  vk::Bool32(true), UINT64_MAX);

  assert(result == vk::Result::eSuccess &&
         "failed to wait for the in Flight fence");
  result = g_vkContext->device.resetFences(1, &m_frameInflight);
  assert(result == vk::Result::eSuccess && "failed to reset in flight fence");

  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
}

void Renderer::recordCommandBuffer(int imageIndex) {
  vk::Result result;
  vk::CommandBufferBeginInfo beginInfo{};
  vk::ClearValue clearcolourValue{};

  clearcolourValue.color = {173 / 255.0f, 216 / 255.0f, 230 / 255.0f,
                            1.0f}; // light blue

  vk::ClearValue cleardepthvalue{};
  cleardepthvalue.depthStencil = vk::ClearDepthStencilValue{1.0f, 0};

  std::array<vk::ClearValue, 2> clearvalues{clearcolourValue, cleardepthvalue};

  result = m_commandBuffer.begin(&beginInfo);
  assert(result == vk::Result::eSuccess);

  vk::RenderPassBeginInfo renderPassBeginInfo{};

  renderPassBeginInfo.renderPass = m_renderPass->getHandle();
  renderPassBeginInfo.framebuffer = m_swapChain->getFramebuffer(imageIndex);

  renderPassBeginInfo.renderArea.offset = vk::Offset2D(0, 0);
  renderPassBeginInfo.renderArea.extent = m_swapChain->getExtent();

  renderPassBeginInfo.clearValueCount = 2;
  renderPassBeginInfo.pClearValues = clearvalues.data();

  m_commandBuffer.beginRenderPass(&renderPassBeginInfo,
                                  vk::SubpassContents::eInline);

  m_commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                               m_pipeline->getHandle());

  PushConstant pushconstants;
  pushconstants.rotationData = rotation;
  pushconstants.viewproj = camera->getViewProj();

  m_commandBuffer.pushConstants(m_pipelineLayout->getHandle(),
                                vk::ShaderStageFlagBits::eVertex, 0,
                                sizeof(pushconstants), &pushconstants);

  std::array<vk::DeviceSize, 2> offsets = {0, 0};
  std::array<vk::Buffer, 2> vbuffers = {m_vertexBuffer->getHandle(),
                                        m_instanceBuffer->getHandle()};
  m_commandBuffer.bindVertexBuffers(0, vbuffers.size(), vbuffers.data(),
                                    offsets.data());
  m_commandBuffer.bindIndexBuffer(m_indexBuffer->getHandle(), 0,
                                  vk::IndexType::eUint32);
  m_commandBuffer.drawIndexed(cubeIndices.size(), m_instanceCount, 0, 0, 0);

  ImGui::Render();
  auto draw_data = ImGui::GetDrawData();

  const bool is_minimized =
      (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
  if (!is_minimized) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_commandBuffer);
  }

  m_commandBuffer.endRenderPass();
  m_commandBuffer.end();
}

void Renderer::drawFrame() {

  if (instances.size() > m_maxInstanceCount) {

    m_instanceBuffer->resize(instances.size() * sizeof(InstanceData));
    m_maxInstanceCount = instances.size();
  }
  m_instanceBuffer->update(instances);
  auto imageIndex = g_vkContext->device.acquireNextImageKHR(
      m_swapChain->getHandle(), UINT64_MAX, m_imageAvailable);

  vk::resultCheck(imageIndex.result, "");

  m_commandBuffer.reset();
  recordCommandBuffer(imageIndex.value);

  vk::SubmitInfo submitInfo{};

  vk::Semaphore waitSemaphores[] = {m_imageAvailable};

  vk::PipelineStageFlags waitStages[] = {
      vk::PipelineStageFlagBits::eColorAttachmentOutput};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &m_commandBuffer;

  vk::Semaphore signalSephamores[] = {m_renderFinished};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSephamores;

  auto res = g_vkContext->graphicsQueue.submit(1, &submitInfo, m_frameInflight);
  assert(res == vk::Result::eSuccess);

  vk::SubpassDependency dep{};
  dep.srcSubpass = VK_SUBPASS_EXTERNAL;
  dep.dstSubpass = 0;

  dep.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

  dep.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
  dep.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

  vk::PresentInfoKHR presentInfo{};

  vk::SwapchainKHR swapChains[] = {m_swapChain->getHandle()};

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &m_renderFinished;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex.value;
  presentInfo.pResults = nullptr;
  res = g_vkContext->presentQueue.presentKHR(&presentInfo);
  assert(res == vk::Result::eSuccess);
}

Renderer::~Renderer() {

  vk::Result res = g_vkContext->device.waitForFences(
      1, &m_frameInflight, vk::Bool32(true), UINT64_MAX);
  assert(res == vk::Result::eSuccess);

  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  g_vkContext->device.destroyCommandPool(m_commandPool);
  g_vkContext->device.destroySemaphore(m_imageAvailable);
  g_vkContext->device.destroySemaphore(m_renderFinished);
  g_vkContext->device.destroyFence(m_frameInflight);
  g_vkContext->device.destroyDescriptorPool(imGuiDescriptorPool);
}

void Renderer::addInstance(glm::mat4 model, glm::vec4 colour) {

  instances.push_back({model, colour});
  m_instanceCount++;

  assert(m_instanceCount <= m_maxInstanceCount);
}

void Renderer::setMaxInstanceCount(int maxInstanceCount) {
  m_maxInstanceCount = maxInstanceCount;
  m_instanceBuffer = std::make_unique<InstanceBuffer>(m_maxInstanceCount *
                                                      sizeof(InstanceData));
  // the old buffer should be destroyed
  // TODO: look into a copy command with a command buffer
  instances.clear();
  // this may be inneficient TODO: optimisation

  m_instanceCount = 0;
}

#include "Graphics/RenderPass.h"
#include "Core/Globals.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"
using namespace TerraMorph::Core;
using namespace TerraMorph::Graphics;

RenderPass::RenderPass(vk::Format imageFormat) {

  vk::AttachmentDescription colourAttachment{};
  colourAttachment.format = imageFormat;
  colourAttachment.samples = vk::SampleCountFlagBits::e1;
  colourAttachment.loadOp = vk::AttachmentLoadOp::eClear;
  colourAttachment.storeOp = vk::AttachmentStoreOp::eStore;
  colourAttachment.initialLayout = vk::ImageLayout::eUndefined;
  colourAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

  vk::AttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

  vk::AttachmentDescription depthAttachment{};
  depthAttachment.format = vk::Format::eD32Sfloat;
  depthAttachment.samples = vk::SampleCountFlagBits::e1;
  depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
  depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
  depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
  depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
  depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
  depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

  vk::AttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

  vk::SubpassDescription subpass{};
  subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  vk::RenderPassCreateInfo createInfo{};

  std::array<vk::AttachmentDescription, 2> attachments = {colourAttachment,
                                                          depthAttachment};

  createInfo.attachmentCount = attachments.size();
  createInfo.pAttachments = attachments.data();
  createInfo.subpassCount = 1;
  createInfo.pSubpasses = &subpass;

  auto res =
      g_vkContext->device.createRenderPass(&createInfo, nullptr, &m_handle);

  vk::resultCheck(res, "error, renderPass creation failed");
}

RenderPass::~RenderPass() { g_vkContext->device.destroyRenderPass(m_handle); }
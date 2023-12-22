#include "Graphics/Swapchain.h"
#include "Core/Globals.h"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"
#include <SDL_video.h>

using namespace TerraMorph::Graphics;
using namespace TerraMorph::Core;

vk::SurfaceFormatKHR Swapchain::chooseSurfaceFormat() {

  vk::Bool32 supported;
  int graphicsIndex = Core::g_vkContext->queueFamilyIndices["Graphics"].value();
  vk::Result res = Core::g_vkContext->physicalDevice.getSurfaceSupportKHR(
      graphicsIndex, Core::g_vkContext->surface, &supported);

  if (!supported) {
    assert(false);
  }

  auto availableFormats =
      Core::g_vkContext->physicalDevice.getSurfaceFormatsKHR(
          Core::g_vkContext->surface);
  for (auto &availableFormat : availableFormats) {
    if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
        availableFormat.colorSpace == vk::ColorSpaceKHR::eDisplayNativeAMD)
    // TODO research why this colour space looks the best for mac

    {
      return availableFormat;
    }
  }
  return availableFormats[0];
}
vk::PresentModeKHR Swapchain::choosePresentMode() {
  std::vector<vk::PresentModeKHR> modes =
      Core::g_vkContext->physicalDevice.getSurfacePresentModesKHR(
          Core::g_vkContext->surface);
  for (const auto &availablePresentMode : modes) {
    if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
      return availablePresentMode;
    }
  }

  return vk::PresentModeKHR::eFifo;
}
vk::Extent2D Swapchain::chooseExtent(SDL_Window *window) {
  auto capabilities =
      Core::g_vkContext->physicalDevice.getSurfaceCapabilitiesKHR(
          Core::g_vkContext->surface);

  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    int width, height;
    SDL_GL_GetDrawableSize(window, &width, &height);

    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

Swapchain::Swapchain(SDL_Window *win) {

  vk::SwapchainCreateInfoKHR createInfo{};

  vk::SurfaceFormatKHR format = chooseSurfaceFormat();
  m_imageFormat = format.format;

  vk::SurfaceCapabilitiesKHR surfaceCapabilities =
      g_vkContext->physicalDevice.getSurfaceCapabilitiesKHR(
          g_vkContext->surface);

  m_extent = chooseExtent(win);
  createInfo.imageFormat = format.format;
  createInfo.imageColorSpace = format.colorSpace;
  createInfo.minImageCount = surfaceCapabilities.minImageCount + 1;

  createInfo.imageExtent = m_extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
  createInfo.presentMode = choosePresentMode();
  createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
  createInfo.preTransform = g_vkContext->physicalDevice
                                .getSurfaceCapabilitiesKHR(g_vkContext->surface)
                                .currentTransform;

  createInfo.clipped = 1;
  createInfo.surface = g_vkContext->surface;
  createInfo.oldSwapchain = nullptr;

  vk::resultCheck(
      g_vkContext->device.createSwapchainKHR(&createInfo, nullptr, &m_handle),
      "");

  m_images = g_vkContext->device.getSwapchainImagesKHR(m_handle);

  m_imageViews.resize(m_images.size());

  int index = 0;
  for (auto &i : m_images) {
    vk::ImageViewCreateInfo info{};
    info.image = i;
    info.viewType = vk::ImageViewType::e2D;
    info.format = format.format;

    info.components.r = vk::ComponentSwizzle::eIdentity;
    info.components.g = vk::ComponentSwizzle::eIdentity;
    info.components.b = vk::ComponentSwizzle::eIdentity;
    info.components.a = vk::ComponentSwizzle::eIdentity;

    info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;

    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;
    vk::Result res = g_vkContext->device.createImageView(&info, nullptr,
                                                         &m_imageViews[index]);
    if (res != vk::Result::eSuccess) {
      assert(false);
    }
    index++;
  }
}

void Swapchain::createFrameBuffers(vk::RenderPass rp) {

  m_framebuffers.resize(m_imageViews.size());

  for (size_t i = 0; i < m_imageViews.size(); i++) {
    vk::ImageView attachments[] = {m_imageViews[i]};
    vk::FramebufferCreateInfo frameBufferInfo{};
    frameBufferInfo.renderPass = rp;
    frameBufferInfo.attachmentCount = 1;
    frameBufferInfo.pAttachments = attachments;
    frameBufferInfo.width = m_extent.width;
    frameBufferInfo.height = m_extent.height;
    frameBufferInfo.layers = 1;
    vk::resultCheck(g_vkContext->device.createFramebuffer(
                        &frameBufferInfo, nullptr, &m_framebuffers[i]),
                    "");
  }
}

Swapchain::~Swapchain() {

    // NOTE , this could potentially be done all in one loop
  for (auto &i : m_framebuffers) {
    g_vkContext->device.destroyFramebuffer(i);
  }

  for (auto &i : m_imageViews) {
    g_vkContext->device.destroyImageView(i);
  }

  g_vkContext->device.destroySwapchainKHR(m_handle);
}
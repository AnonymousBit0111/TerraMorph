#include "Graphics/Swapchain.h"
#include "Core/Globals.h"
#include "Graphics/VkContext.h"
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
        availableFormat.colorSpace == vk::ColorSpaceKHR::eDisplayNativeAMD) {
      // TODO research why this colour space looks the best for mac

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
  m_depthImageViews.resize(m_images.size());

  m_depthImages.resize(m_images.size());

  m_depthImageMemory.resize((m_images.size()));

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

    vk::ImageViewCreateInfo DepthInfo{};
    DepthInfo.image = i;
    info.viewType = vk::ImageViewType::e2D;
    info.format = vk::Format::eD32Sfloat;
    info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.layerCount = 1;

    vk::ImageCreateInfo imageInfo{};
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.format = vk::Format::eD32Sfloat;
    imageInfo.extent.width = m_extent.width;
    imageInfo.extent.height = m_extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = vk::SampleCountFlagBits::e1;
    imageInfo.tiling = vk::ImageTiling::eOptimal;
    imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;

    m_depthImages[index] = g_vkContext->device.createImage(imageInfo);
    vk::MemoryRequirements memoryRequirements =
        g_vkContext->device.getImageMemoryRequirements(m_depthImages[index]);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex =
        VKContext::findMemoryType(memoryRequirements.memoryTypeBits,
                                  vk::MemoryPropertyFlagBits::eDeviceLocal);

    m_depthImageMemory[index] = g_vkContext->device.allocateMemory(allocInfo);

    g_vkContext->device.bindImageMemory(m_depthImages[index],
                                        m_depthImageMemory[index], 0);

    vk::ImageViewCreateInfo viewInfo{};
    viewInfo.image = m_depthImages[index];
    viewInfo.viewType = vk::ImageViewType::e2D;
    viewInfo.format = vk::Format::eD32Sfloat;
    viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    m_depthImageViews[index] = g_vkContext->device.createImageView(viewInfo);

    index++;
  }
}

void Swapchain::createFrameBuffers(vk::RenderPass rp) {

  m_framebuffers.resize(m_imageViews.size());

  for (size_t i = 0; i < m_imageViews.size(); i++) {
    vk::ImageView attachments[] = {m_imageViews[i], m_depthImageViews[i]};
    vk::FramebufferCreateInfo frameBufferInfo{};
    frameBufferInfo.renderPass = rp;
    frameBufferInfo.attachmentCount =
        2; // i know the depth image wont be used , this is for testing
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

  for (auto &i : m_depthImageViews) {
    g_vkContext->device.destroyImageView(i);
  }

  for (auto &i : m_depthImages) {
    g_vkContext->device.destroyImage(i);
  }

  for (auto &i : m_depthImageMemory) {
    g_vkContext->device.freeMemory(i);
  }

  g_vkContext->device.destroySwapchainKHR(m_handle);
}
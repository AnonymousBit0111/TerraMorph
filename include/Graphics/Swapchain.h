#pragma once
#include <SDL_video.h>
#include <vector>
#include <vulkan/vulkan.hpp>
namespace TerraMorph {
namespace Graphics {

class Swapchain {
private:
  static vk::SurfaceFormatKHR chooseSurfaceFormat();
  static vk::PresentModeKHR choosePresentMode();
  static vk::Extent2D chooseExtent(SDL_Window *window);

  vk::Extent2D m_extent;
  vk::Format m_imageFormat;

  std::vector<vk::ImageView> m_imageViews;
  std::vector<vk::Image> m_images;
  std::vector<vk::Framebuffer> m_framebuffers;

  vk::SwapchainKHR m_handle;

public:
  Swapchain(SDL_Window *window);
  vk::Format getImageFormat() const { return m_imageFormat; }
  vk::Extent2D getExtent() const { return m_extent; }
  void createFrameBuffers(vk::RenderPass renderpass);
  ~Swapchain();
};

} // namespace Graphics
} // namespace TerraMorph
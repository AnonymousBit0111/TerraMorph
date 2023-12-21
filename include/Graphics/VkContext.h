#pragma once
#include <SDL.h>
#include <map>
#include <memory>

#include <vulkan/vulkan.hpp>

namespace TerraMorph {
namespace Graphics {
struct VKContext {
  vk::Instance instance;
  vk::Device device;
  vk::SurfaceKHR surface;
  vk::PhysicalDevice physicalDevice;
  vk::Queue graphicsQueue;
  vk::Queue presentQueue;
  vk::Fence inFlightFence;
  vk::Semaphore ImageAvailable;
  vk::Semaphore RenderFinished;
  std::map<std::string, std::optional<uint32_t>> queueFamilyIndices;
  std::vector<const char *> validationLayers;
  SDL_Window *window;

  bool destroyed = false;

  VKContext(SDL_Window *window);
  // Destructor
  ~VKContext();
  void destroy();

private:
  void createInstance();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
};


} // namespace Graphics
} // namespace TerraMorph
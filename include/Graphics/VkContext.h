#pragma once
#include "glm/fwd.hpp"
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
  std::map<std::string, std::optional<uint32_t>> queueFamilyIndices;
  std::vector<const char *> validationLayers;
  SDL_Window *window;

  bool destroyed = false;

  VKContext(SDL_Window *window);
  // Destructor
  ~VKContext();
  void destroy();
  static glm::uint32_t findMemoryType(uint32_t typeFilter,
                                      vk::MemoryPropertyFlags properties);

private:
  void createInstance();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
};

} // namespace Graphics
} // namespace TerraMorph
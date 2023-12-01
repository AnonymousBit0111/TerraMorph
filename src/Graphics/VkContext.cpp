#include "Graphics/VkContext.h"
#include "Core/Logging.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"
#include <SDL_video.h>
#include <SDL_vulkan.h>
#include <cassert>

using namespace TerraMorph::Graphics;
using namespace TerraMorph::Core::Logging;

void VKContext::createInstance() {
  vk::ApplicationInfo appInfo{};

  appInfo.pApplicationName = "TerraMorph";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "CustomEngine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_2;

  vk::InstanceCreateInfo createInfo{};
  createInfo.pApplicationInfo = &appInfo;

  // Request validation layers for debugging
  std::vector<vk::LayerProperties> availableLayers =
      vk::enumerateInstanceLayerProperties();

  std::vector<std::string> requestedLayers = {"VK_LAYER_KHRONOS_validation"};
  std::vector<const char *> receivedLayers;
  for (auto &i : requestedLayers) {
    for (auto &layer : availableLayers) {
      if (std::string(layer.layerName) == std::string(i)) {
        Core::Logging::log("validation layer: " + i +
                           " is available and supported");

        receivedLayers.push_back(layer.layerName);
      }
    }
  }

  createInfo.enabledLayerCount = receivedLayers.size();
  createInfo.ppEnabledLayerNames = receivedLayers.data();

  validationLayers = receivedLayers;

  uint32_t extensionCount = 0;
  if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr) ||
      extensionCount == 0) {
    assert(false && "Failed to get the number of Vulkan instance extensions.");
  }
  std::vector<const char *> extensions(extensionCount);

  if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount,
                                        extensions.data())) {
    assert(false && "Failed to get Vulkan instance extensions.");
  }

  // this is needed for mac
  createInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;

  extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  for (auto &reqExt : extensions) {

    for (auto &i : vk::enumerateInstanceExtensionProperties()) {
      if (i.extensionName == std::string(reqExt)) {
        log("instance extension: " + std::string(i.extensionName) +
            " is supported");
      }
    }
  }

  createInfo.enabledExtensionCount = extensions.size();

  createInfo.ppEnabledExtensionNames = extensions.data();

  for (auto &i : extensions) {
    log("enabled instance extension:" + std::string(i));
  }

  auto res = vk::createInstance(&createInfo, nullptr, &this->instance);

  vk::resultCheck(res, "error:");

  Core::Logging::log("succesfully created instance");
}

void VKContext::createSurface() {

  // Since SDL2 doesnt use vulkan.hpp , i will have to create a vk::SurfaceKHR
  // from a VKSurfaceKHR
  VkSurfaceKHR Csurf;
  if (!SDL_Vulkan_CreateSurface(window, this->instance, &Csurf)) {
    assert(false && "Failed to create Vulkan surface.");
  }
  surface = Csurf;
}
VKContext::VKContext(SDL_Window *pwindow) : window(pwindow) {
  createInstance();
  createSurface();
}

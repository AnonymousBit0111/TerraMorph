#include "Graphics/VkContext.h"
#include "Core/Logging.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
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

void VKContext::pickPhysicalDevice() {
  std::vector<vk::PhysicalDevice> physicalDevices =
      instance.enumeratePhysicalDevices();

  vk::PhysicalDevice currentDevice = physicalDevices[0];

  for (auto &i : physicalDevices) {
    vk::PhysicalDeviceProperties props = i.getProperties();
    vk::PhysicalDeviceFeatures features = i.getFeatures();

    // this will just pick the last discrete gpu on the list , it shouldnt be an
    // issue though since very few computers have many gpus, i will ammend when
    // neccessary to save time
    if (props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
      currentDevice = i;
    }
  }
  this->physicalDevice = currentDevice;
  log("found usable GPU");
}

void VKContext::createLogicalDevice() {

  std::vector<vk::QueueFamilyProperties> queueFamilies =
      this->physicalDevice.getQueueFamilyProperties();

  uint32_t i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
      this->queueFamilyIndices["Graphics"] = i;
    }
    bool presentSupport;
    presentSupport = this->physicalDevice.getSurfaceSupportKHR(i, surface);
    if (presentSupport) {
      this->queueFamilyIndices["Present"] = i;
    }
    i++;
  }

  if (this->queueFamilyIndices.count("Graphics") < 0) {
    error("failed to find Graphics Queue");
  }

  vk::DeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.queueFamilyIndex =
      this->queueFamilyIndices["Graphics"].value();
  queueCreateInfo.queueCount = 1;
  static float queuePriority = 1.0f;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  vk::PhysicalDeviceFeatures deviceFeatures{};

  std::vector<vk::ExtensionProperties> availableExtensions =
      this->physicalDevice.enumerateDeviceExtensionProperties();

  std::vector<const char *> enabledExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset"};

  for (auto &req : enabledExtensions) {
    bool extFound = false;
    for (auto &i : this->physicalDevice.enumerateDeviceExtensionProperties()) {

      if (std::string(i.extensionName) == std::string(req)) {
        log("device extension " + std::string(i.extensionName) +
            " is enabled and supported");
        extFound = true;
        break;
      }
    }
    if (!extFound) {
      warn("device extension " + std::string(req) + " not found");
    }
  }

  vk::DeviceCreateInfo createInfo{};
  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(enabledExtensions.size());
  createInfo.ppEnabledExtensionNames = enabledExtensions.data();
  createInfo.queueCreateInfoCount = 1;
  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.pEnabledFeatures = &deviceFeatures;

  this->device = this->physicalDevice.createDevice(createInfo);

  this->graphicsQueue =
      this->device.getQueue(this->queueFamilyIndices["Graphics"].value(), 0);
  this->presentQueue =
      this->device.getQueue(this->queueFamilyIndices["Present"].value(), 0);
  log("succesfully created logical device");
}

VKContext::VKContext(SDL_Window *pwindow) : window(pwindow) {
  createInstance();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
}

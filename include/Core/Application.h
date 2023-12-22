#pragma once

#include "Core/Window.h"
#include "Graphics/Renderer.h"
#include "Graphics/VkContext.h"
#include <SDL.h>
#include <memory>
namespace TerraMorph {
namespace Core {
class Application {
public:
  static std::shared_ptr<Window> window;
  static std::shared_ptr<Graphics::Renderer> renderer;
  static bool open;


  static void init();
  static void initVulkan();
  static void run();
  static void cleanup();
  static void quit(TerraMorph::Core::EventInfo info) {
    cleanup();
    exit(0);
  }
};

} // namespace Core
} // namespace TerraMorph
#pragma once

#include "Core/Window.h"
#include "Graphics/VkContext.h"
#include <SDL.h>
#include <memory>
namespace TerraMorph {
namespace Core {
class Application {
private:
  std::shared_ptr<Window> window = nullptr;
  std::shared_ptr<Graphics::VKContext> g_vkContext = nullptr;
  bool open;
  static void quit(TerraMorph::Core::EventInfo info) {
    SDL_Quit();
    exit(0);
  }

public:
  void init();
  void initVulkan();
  void run();
  void cleanup();
};

} // namespace Core
} // namespace TerraMorph
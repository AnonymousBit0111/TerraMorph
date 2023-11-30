#pragma once

#include "Core/Window.h"
#include <SDL.h>
#include <memory>
namespace TerraMorph {
namespace Core {
class Application {
private:
  std::shared_ptr<Window> window = nullptr;
  bool open;
  static void quit(TerraMorph::Core::EventInfo info) {
    SDL_Quit();
    exit(0);
  }

public:
  void init();
  void run();
  void cleanup();
};

} // namespace Core
} // namespace TerraMorph
#pragma once

#include "Core/Window.h"
#include <SDL.h>
#include <memory>
namespace TerraMorph {
namespace Core {
class Application {
private:
  std::shared_ptr<Window> window = nullptr;
  bool Open;
  static void Quit(TerraMorph::Core::EventInfo info) {
    SDL_Quit();
    exit(0);
  }

public:
  void Init();
  void Run();
  void Cleanup();
};

} // namespace Core
} // namespace TerraMorph
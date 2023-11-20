#pragma once

#include "Core/Event.h"
#include "EventHandlers.h"
#include <SDL2/SDL.h>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <vector>
namespace TerraMorph {
namespace Core {

class Window {
private:
  uint m_width, m_height = 0;
  std::string m_name;
  SDL_Window *m_window = nullptr;
  std::unordered_multimap<EventType, EventHandler> m_eventHandlers;

public:
  Window(const std::string &name, uint width, uint height);

  void subscribeToEvent(EventType event, EventHandler handler);

  void pollEvents();
  ~Window();
};


} // namespace Core
} // namespace TerraMorph
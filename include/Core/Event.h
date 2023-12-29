#pragma once

#include <SDL_events.h>
namespace TerraMorph {
namespace Core {

enum class EventType {
  Quit = SDL_QUIT,
  KeyDown = SDL_KEYDOWN,
  KeyUp = SDL_KEYUP,
  MouseMoved = SDL_MOUSEMOTION
};
}
} // namespace TerraMorph
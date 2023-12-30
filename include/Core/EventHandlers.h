
#pragma once

#include "Core/Event.h"
#include "glm/glm.hpp"
#include <SDL_keycode.h>
namespace TerraMorph {
namespace Core {
struct EventInfo {
  EventType type;
  SDL_Keycode key;
  glm::vec2 mouseMotion;
};
typedef void (*EventHandler)(EventInfo);

} // namespace Core
} // namespace TerraMorph
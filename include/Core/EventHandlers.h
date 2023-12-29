
#pragma once

#include "Core/Event.h"
#include "glm/glm.hpp"
namespace TerraMorph {
namespace Core {

struct EventInfo {
  EventType type;
  glm::vec2 mouseMotion;
};
typedef void (*EventHandler)(EventInfo);

} // namespace Core
} // namespace TerraMorph
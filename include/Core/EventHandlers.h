
#pragma once

#include "Core/Event.h"
namespace TerraMorph {
namespace Core {

struct EventInfo {
  EventType type;
};
typedef void (*EventHandler)(EventInfo);

} // namespace Core
} // namespace TerraMorph
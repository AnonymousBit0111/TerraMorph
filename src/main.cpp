#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <iostream>

#include "Core/Event.h"
#include "Core/EventHandlers.h"
#include "Core/Window.h"

void Quit(TerraMorph::Core::EventInfo info) { exit(0); }

int main() {
  TerraMorph::Core::Window window("sup", 123, 123);

  // inititalise SDL , quit if it it doesnt succeed
  if (SDL_Init(SDL_INIT_EVERYTHING)) {
    return -1;
  }
  window.subscribeToEvent(TerraMorph::Core::EventType::Quit, Quit);
  while (true) {
    window.pollEvents();
  }

  SDL_Quit();
}
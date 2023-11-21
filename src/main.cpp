#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <iostream>

#include "Core/Application.h"
#include "Core/Event.h"
#include "Core/EventHandlers.h"
#include "Core/Window.h"


int main() {
  TerraMorph::Core::Application app;
  app.Init();
  app.Run();
}
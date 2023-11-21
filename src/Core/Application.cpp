#include "Core/Application.h"
#include "Core/Window.h"
#include <memory>

using namespace TerraMorph::Core;
void Application::Init() {

  window = std::make_shared<Window>("TerraMorph", 400, 300);
  Open = true;

  window->subscribeToEvent(TerraMorph::Core::EventType::Quit, Quit);
}

void Application::Run() {
  while (Application::Open) {
    window->pollEvents();
  }
}

void Application::Cleanup() { SDL_Quit(); }
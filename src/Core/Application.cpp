#include "Core/Application.h"
#include "Core/Window.h"
#include "Graphics/VkContext.h"
#include <memory>

using namespace TerraMorph::Core;
void Application::init() {

  window = std::make_shared<Window>("TerraMorph", 400, 300);
  open = true;

  window->subscribeToEvent(TerraMorph::Core::EventType::Quit, quit);


  initVulkan();
}

void Application::initVulkan(){
  g_vkContext = std::make_shared<Graphics::VKContext>(window->getRawHandle());
  
}

void Application::run() {
  while (Application::open) {
    window->pollEvents();
  }
}


void Application::cleanup() { SDL_Quit(); }
#include "Core/Application.h"
#include "Core/Globals.h"
#include "Core/Window.h"
#include "Graphics/Renderer.h"
#include "Graphics/VkContext.h"
#include "imgui.h"
#include <memory>

using namespace TerraMorph::Core;
using namespace TerraMorph::Graphics;

std::shared_ptr<Window> Application::window = nullptr;
std::shared_ptr<Renderer> Application::renderer = nullptr;
bool Application::open = false;
void Application::init() {

  window = std::make_shared<Window>("TerraMorph", 1080, 720);
  open = true;

  window->subscribeToEvent(TerraMorph::Core::EventType::Quit, quit);

  initVulkan();
}
void Application::initVulkan() {
  g_vkContext = std::make_shared<Graphics::VKContext>(window->getRawHandle());
  renderer = std::make_shared<Renderer>(window->getRawHandle());
}

void Application::run() {
  while (Application::open) {
    window->pollEvents();

    renderer->beginFrame();

    // Any ImGui call should go here

    ImGui::ShowDemoWindow();
    renderer->drawFrame();
  }
}
void Application::cleanup() {
  renderer.reset(); // to follow vulkan guidelines and make sure all objects are
                    // destroyed before the device
  g_vkContext->destroy();
  SDL_Quit();
}
#include "Core/Application.h"
#include "Core/Globals.h"
#include "Core/Window.h"
#include "Graphics/Camera.h"
#include "Graphics/Renderer.h"
#include "Graphics/VkContext.h"
#include "glm/ext/matrix_transform.hpp"
#include "imgui.h"
#include <memory>

using namespace TerraMorph::Core;
using namespace TerraMorph::Graphics;

std::shared_ptr<Window> Application::window = nullptr;
std::shared_ptr<Renderer> Application::renderer = nullptr;

Camera Application::camera = Camera(glm::vec2(500, 500), glm::vec3(0, 0, 11));
bool Application::open = false;
void Application::init() {

  window = std::make_shared<Window>("TerraMorph", 1080, 720);
  open = true;

  window->subscribeToEvent(TerraMorph::Core::EventType::Quit, quit);

  initVulkan();
}
void Application::initVulkan() {
  g_vkContext = std::make_shared<Graphics::VKContext>(window->getRawHandle());
  renderer = std::make_shared<Renderer>(window->getRawHandle(), &camera);
}

void Application::UIcalls() {

  ImGui::Begin("sup");
  ImGui::End();

  static float x, y, z, px, py, pz, scale, rot = 0.0f;

  if (ImGui::IsKeyDown(ImGuiKey_W)) {
    camera.move(glm::vec3(0, 0, 1));
  }
  if (ImGui::IsKeyDown(ImGuiKey_A)) {
    camera.move(glm::vec3(-1, 0, 0));
  }
  if (ImGui::IsKeyDown(ImGuiKey_S)) {
    camera.move(glm::vec3(0, 0, -1));
  }
  if (ImGui::IsKeyDown(ImGuiKey_D)) {
    camera.move(glm::vec3(1, 0, 0));
  }
  if (ImGui::IsKeyDown(ImGuiKey_UpArrow)) {
    camera.move(glm::vec3(0, 1, 0));
  }

  if (ImGui::IsKeyDown(ImGuiKey_DownArrow)) {
    camera.move(glm::vec3(0, -1, 0));
  }

  ImGui::Begin("test");
  ImGui::InputFloat("move.x", &x, 1.0f, 10.0f);
  ImGui::InputFloat("move.y", &y, 1.0f, 10.0f);
  ImGui::InputFloat("move.z", &z, 1.0f, 10.0f);
  ImGui::InputFloat("scale", &scale, 1.0f, 10.0f);
  ImGui::InputFloat("rotation", &rot);

  ImGui::Text("position.x: %f", camera.getPosition().x);
  ImGui::Text("position.y:%f", camera.getPosition().y);
  ImGui::Text("position.z:%f", camera.getPosition().z);

  if (ImGui::Button("applymovement")) {

    camera.move(glm::vec3(x, y, z));
  }

  static bool rotating = false;
  if (ImGui::Button("applyrotation")) {
    rotating = true;
  }

  if (rotating) {
    // rotation = glm::rotate(rotation, glm::radians(rot), glm::vec3(0, 1, 0));

    renderer->setData(glm::rotate(renderer->getData(), glm::radians(rot),
                                  glm::vec3(0, 1, 0)));
  }
  ImGui::End();
}
void Application::run() {
  while (Application::open) {
    window->pollEvents();

    renderer->beginFrame();

    // Any ImGui call should go here

    UIcalls();

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

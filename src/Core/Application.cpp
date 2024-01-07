#include "Core/Application.h"
#include "Core/Event.h"
#include "Core/EventHandlers.h"
#include "Core/Globals.h"
#include "Core/Logging.h"
#include "Core/Window.h"
#include "Graphics/Camera.h"
#include "Graphics/Renderer.h"
#include "Graphics/VkContext.h"
#include "Terrain/Cube.h"
#include "Terrain/Noise.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "imgui.h"
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_stdinc.h>
#include <cstdlib>
#include <memory>

using TerraMorph::Terrain::Cube;

using namespace TerraMorph::Core;
using namespace TerraMorph::Graphics;

std::shared_ptr<Window> Application::window = nullptr;
std::shared_ptr<Renderer> Application::renderer = nullptr;
bool Application::captureMouse = false;

int Application::octaves = 2;
float Application::persistance = 0.5;
float Application::lacunarity = 2;
std::vector<TerraMorph::Terrain::Cube> Application::cubes;

Camera Application::camera = Camera(glm::vec2(1080, 720), glm::vec3(0, 0, 11));
bool Application::open = false;
void Application::init() {

  window = std::make_shared<Window>("TerraMorph", 1080, 720);
  open = true;

  window->subscribeToEvent(TerraMorph::Core::EventType::Quit, quit);
  window->subscribeToEvent(TerraMorph::Core::EventType::MouseMoved, mouseMoved);
  window->subscribeToEvent(Core::EventType::KeyDown, keyPressed);

  initVulkan();

  auto instanceModels = renderer->getInstanceModels();
  int index = 0;
  float scale = 0.1f;
  float blockscale = 2.0f;

  auto noiseMap = Terrain::Noise::generateNoiseMap(
      glm::vec2(500, 500), 1.0f, octaves, persistance, lacunarity);
  for (int x = 0; x < 500; x++) {
    for (int z = 0; z < 500; z++) {
      glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));

      renderer->addInstance(model, glm::vec4(1.0f / (rand() % 18),
                                             1.0f / (rand() % 18),
                                             1.0f / (rand() % 18), 1.0f));

      // renderer->addInstance(model, glm::vec4(1.0f));
      Cube cube(glm::vec3(x * blockscale, noiseMap[x][z] * 100, z * blockscale),
                &instanceModels->at(index)); // copying might have happened

      cubes.push_back(cube);
      index++;
    }
  }
}

void Application::keyPressed(EventInfo info) {

  switch (info.key) {
  case SDLK_ESCAPE:
    captureMouse = !captureMouse;
    if (captureMouse) {
      SDL_SetWindowGrab(window->getRawHandle(), SDL_TRUE);
      if (SDL_ShowCursor(SDL_DISABLE) == -1) {
        quit(EventInfo{});
      };
    } else {
      SDL_SetWindowGrab(window->getRawHandle(), SDL_FALSE);
      SDL_ShowCursor(SDL_ENABLE);
    }
    break;

  default:
    break;
  }
}
void Application::initVulkan() {
  g_vkContext = std::make_shared<Graphics::VKContext>(window->getRawHandle());
  renderer =
      std::make_shared<Renderer>(window->getRawHandle(), &camera, 500 * 500);
}

void Application::UIcalls() {

  static float x, y, z, px, py, pz, scale, rot = 0.0f;

  if (captureMouse) {
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
  }

  ImGui::Begin("Info");
  ImGui::Text("position.x: %f", camera.getPosition().x);
  ImGui::Text("position.y:%f", camera.getPosition().y);
  ImGui::Text("position.z:%f", camera.getPosition().z);
  ImGui::End();

  ImGui::Begin("Terrain info");

  ImGui::Text("Octaves: %d\nPersistance: %f\nLacunarity %f\n", octaves,
              persistance, lacunarity);

  bool changed = ImGui::SliderFloat("persistance:", &persistance, 0.0f, 10.0f) |
                 ImGui::SliderFloat("lacunarity", &lacunarity, 0.0f, 10.0f) |
                 ImGui::InputInt("Octaves", &octaves);

  if (changed) {
    regenTerrain();
  }

  ImGui::End();
}

void Application::mouseMoved(TerraMorph::Core::EventInfo info) {

  if (captureMouse) {

    camera.calculateDirection(info.mouseMotion);
  }
}
void Application::run() {
  while (Application::open) {
    window->pollEvents();

    renderer->beginFrame();

    UIcalls();

    renderer->drawFrame();
  }
}
void Application::cleanup() {
  renderer.reset(); // to follow vulkan guidelines and make sure all objects are
                    // destroyed before the device
  g_vkContext->destroy();
  SDL_Quit();
}

void Application::regenTerrain() {

  float blockscale = 2.0f;
  int index = 0;

  auto instanceModels = renderer->getInstanceModels();
  auto noiseMap = Terrain::Noise::generateNoiseMap(
      glm::vec2(500, 500), 1.0f, octaves, persistance, lacunarity);
  for (int x = 0; x < 500; x++) {
    for (int z = 0; z < 500; z++) {

      glm::vec3 currentpos = cubes[index].getPosition();
      currentpos.y = noiseMap[x][z] * 100;
      instanceModels->at(index).model =
          glm::translate(glm::mat4(1.0f), currentpos);

      float red = currentpos.y / 100;

      if (red > 0) {
        instanceModels->at(index).colour = glm::vec4(0.0f, 0.0f, red, 1.0f);

      } else {
        instanceModels->at(index).colour =
            glm::vec4(red * -1, 0.0f, 0.0f, 1.0f);
      }

      index++;
    }
  }
}
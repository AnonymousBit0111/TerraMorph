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
#include "glm/gtx/matrix_decompose.hpp"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_stdinc.h>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using TerraMorph::Terrain::Cube;
using namespace TerraMorph::Core;
using namespace TerraMorph::Graphics;

std::shared_ptr<Window> Application::window = nullptr;
std::shared_ptr<Renderer> Application::renderer = nullptr;
bool Application::captureMouse = false;

int Application::octaves, Application::mapSizex, Application::mapSizez = 2;
float Application::persistance = 0.5;
float Application::lacunarity = 2;
std::vector<TerraMorph::Terrain::Cube> Application::cubes;

Camera Application::camera = Camera(glm::vec2(1080, 720), glm::vec3(0, 0, 11));
bool Application::open = false;
void Application::init() {

  mapSizex = 500;
  mapSizez = 500;

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

  ImGui::InputInt("Map Size X", &mapSizex);
  ImGui::InputInt("Map Size Z", &mapSizez);

  if (ImGui::Button("Resize map")) {
    resizeMap();
  }

  if (changed) {
    auto noiseMap = Terrain::Noise::generateNoiseMap(
        glm::vec2(mapSizex, mapSizez), 1.0f, octaves, persistance, lacunarity);
    regenTerrain(noiseMap);
  }

  ImGui::End();

  ImGui::Begin("save dialog");
  static std::string path = "";

  ImGui::InputText("FilePath:", &path);
  if (ImGui::Button("save")) {
    assert(path != "");
    Save(path);
    SaveCSV(path);
  }
  ImGui::End();

  ImGui::Begin("load dialogue");

  static std::string loadpath = "";
  ImGui::InputText("LoadPath", &loadpath);
  if (ImGui::Button("load")) {
    assert(loadpath != "");

    LoadCSV(loadpath);
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

void Application::regenTerrain(std::vector<std::vector<float>> &noiseMap) {

  auto instances = renderer->getInstanceModels();

  int index = 0;
  for (int x = 0; x < mapSizex; x++) {
    for (int z = 0; z < mapSizez; z++) {

      glm::mat4 model = glm::translate(
          glm::mat4(1.0f), glm::vec3(x * 2, (noiseMap[x][z] * 100), z * 2));

      float red = noiseMap[x][z];

      glm::vec4 colour = glm::vec4(1.0f);

      if (red > 0) {
        colour = glm::vec4(0.0f, 0.0f, red, 1.0f);

      } else {
        colour = glm::vec4(red * -1, 0.0f, 0.0f, 1.0f);
      }

      instances->at(index) = {model, colour};

      index++;
    }
  }
}

void Application::resizeMap() {
  renderer->setMaxInstanceCount(mapSizex * mapSizez);

  auto noiseMap = Terrain::Noise::generateNoiseMap(
      glm::vec2(mapSizex, mapSizez), 1.0f, octaves, persistance, lacunarity);

  int index = 0;
  for (int x = 0; x < mapSizex; x++) {
    for (int z = 0; z < mapSizez; z++) {

      glm::mat4 model = glm::translate(
          glm::mat4(1.0f), glm::vec3(x * 2, noiseMap[x][z] * 100, z * 2));

      float red = noiseMap[x][z];

      glm::vec4 colour = glm::vec4(1.0f);

      if (red > 0) {
        colour = glm::vec4(0.0f, 0.0f, red, 1.0f);

      } else {
        colour = glm::vec4(red * -1, 0.0f, 0.0f, 1.0f);
      }

      renderer->addInstance(model, colour);

      index++;
    }
  }
}

void Application::Save(const std::string &filePath) {
  std::vector<unsigned char> img(mapSizex * mapSizez * 3);
  auto instancemodels = renderer->getInstanceModels();
  int index = 0;
  float smallestval = 100;
  float largestval = 0;
  for (int x = 0; x < mapSizex; x++) {
    for (int z = 0; z < mapSizez; z++) {

      glm::vec3 scale, skew, position;
      glm::quat rotation;
      glm::vec4 perspective;
      glm::decompose(instancemodels->at(index / 3).model, scale, rotation,
                     position, skew, perspective);

      img[index] = (((((position.y * -1) + 100.0f)) / 200.0f) * 255);
      img[index + 1] = (((((position.y * -1) + 100.0f)) / 200.0f) * 255);
      img[index + 2] = (((((position.y * -1) + 100.0f)) / 200.0f) * 255);

      if (position.y > largestval) {
        largestval = position.y;
      } else if (position.y < smallestval) {
        smallestval = position.y;
      }

      index += 3;
    }
  }

  Logging::log(std::to_string(largestval));
  Logging::log(std::to_string(smallestval));
  stbi_write_png(filePath.c_str(), mapSizex, mapSizez, 3, img.data(),
                 mapSizex * 3);
}

void Application::Load(const std::string &filePath) {

  int width, height, channels;
  unsigned char *img =
      stbi_load(filePath.c_str(), &width, &height, &channels, 0);

  if (img == nullptr) {
    // TODO error handling
    return;
  }

  if (width != mapSizex || height != mapSizez || channels != 3) {
    // TODO error handling
    stbi_image_free(img);
    return;
  }

  std::vector<std::vector<float>> noiseMap;

  noiseMap.resize(mapSizex);

  for (int x = 0; x < mapSizex; x++) {
    for (int z = 0; z < mapSizez; z++) {
      noiseMap.push_back(std::vector<float>());
      noiseMap[x].resize(mapSizez);
      int index = (x + z * mapSizex) * 3;
      float y = (static_cast<float>((img[index]) / 255.0f)) * 200.0f;
      y -= 100.0f;
      y *= -1;

      noiseMap[x][z] = y;
    }
  }

  stbi_image_free(img);

  auto instances = renderer->getInstanceModels();

  int index = 0;
  for (int x = 0; x < mapSizex; x++) {
    for (int z = 0; z < mapSizez; z++) {

      glm::mat4 model = glm::translate(glm::mat4(1.0f),
                                       glm::vec3(x * 2, noiseMap[x][z], z * 2));

      float red = noiseMap[x][z];

      glm::vec4 colour = glm::vec4(1.0f);

      if (red > 0) {
        colour = glm::vec4(0.0f, 0.0f, red / 200.0f, 1.0f);

      } else {
        colour = glm::vec4((red * -1) / 200.0f, 0.0f, 0.0f, 1.0f);
      }

      instances->at(index) = {model, colour};

      index++;
    }
  }
}

void Application::SaveCSV(const std::string &filePath) {

  std::ofstream file(filePath);
  // TODO error handling

  assert(!file.bad());

  auto instancemodels = renderer->getInstanceModels();

  std::string string = "";

  int index = 0;
  for (int x = 0; x < mapSizex; x++) {
    for (int z = 0; z < mapSizez; z++) {

      glm::vec3 scale, skew, position;
      glm::quat rotation;
      glm::vec4 perspective;
      glm::decompose(instancemodels->at(index).model, scale, rotation, position,
                     skew, perspective);

      string += std::to_string((position.y * -1) / 100.0f);
      string += ",";
      index++;
    }
    string += "\n";
  }

  file << string;
  file.close();
}

// Taken from StackOverflow
std::vector<std::string> split(std::string s, std::string delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }

  res.push_back(s.substr(pos_start));
  return res;
}

void Application::LoadCSV(const std::string &filePath) {

  std::ifstream file;
  file.open(filePath);
  std::vector<std::vector<float>> noiseMap;

  assert(!file.bad());

  std::string data = "";

  while (!file.eof()) {

    std::string stringVal = "";

    std::vector<float> noiseVals;
    file >> stringVal;

    auto vals = split(stringVal, ",");

    for (auto &i : vals) {

      if (i == "")
        continue;

      noiseVals.push_back(std::stof(i) * -1);
    }
    noiseMap.push_back(noiseVals);
  }
  regenTerrain(noiseMap);
}
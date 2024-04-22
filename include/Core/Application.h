#pragma once

#include "Core/EventHandlers.h"
#include "Core/Window.h"
#include "Graphics/Camera.h"
#include "Graphics/Renderer.h"
#include "Graphics/VkContext.h"
#include "Terrain/Cube.h"
#include <SDL.h>
#include <memory>
#include <vector>
namespace TerraMorph {
namespace Core {
class Application {
private:
  static void UIcalls();
  static void regenTerrain(std::vector<std::vector<float>> &noiseMap);
  static void resizeMap();
  static void Save(const std::string &filePath);
  static void SaveCSV(const std::string &filePath);
  static void LoadCSV(const std::string &filePath);

public:
  static std::shared_ptr<Window> window;
  static std::shared_ptr<Graphics::Renderer> renderer;
  static Graphics::Camera camera;
  static std::vector<Terrain::Cube> cubes;
  static bool captureMouse;

  static bool open;

  static float  persistance , lacunarity;
  static int octaves, mapSizex,mapSizez;


  static void init();
  static void initVulkan();
  static void run();
  static void cleanup();

  static void mouseMoved(EventInfo info);
  static void quit(EventInfo info) {
    cleanup();
    exit(0);
  }
  static void keyPressed(EventInfo info);
};



} // namespace Core
} // namespace TerraMorph
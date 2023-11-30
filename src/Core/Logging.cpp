#include "Core/Logging.h"
#include <iostream>

using namespace TerraMorph::Core;

void Logging::warn(const std::string &message) {
  std::cout << "WARNING:" << message + "\n";
}

void Logging::error(const std::string &message) {
  std::cerr << "ERROR" << message + "\n";
  exit(-1);
}

void Logging::log(const std::string &message) {
  std::cout << "INFORMATION:" << message + "\n";
}
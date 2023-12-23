#include "Core/FileHandler.h"
#include <cassert>
#include <fstream>

std::vector<char> TerraMorph::Core::FileHandler::readFilebin(const std::string &path) {

  std::ifstream file(path, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    assert(false && "failed to open file!");
  }
  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);
  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();
  return buffer;
}
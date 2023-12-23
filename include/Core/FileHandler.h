

#pragma once

#include <vector>
namespace TerraMorph {

namespace Core {
    namespace FileHandler
    {
        std::vector<char> readFilebin(const std::string& path);
    }
};
} // namespace TerraMorph
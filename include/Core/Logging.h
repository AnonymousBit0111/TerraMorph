#pragma once

#include <string>
namespace TerraMorph {
namespace Core {
namespace Logging {

void warn(const std::string &message);
void error(const std::string &message);
void log(const std::string &message);
} // namespace Logging
} // namespace Core

} // namespace TerraMorph
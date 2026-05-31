#pragma once

#include <string>

namespace RuntimePaths {

void initialize();
std::string persistencePath(const char* relativePath);

}  // namespace RuntimePaths

#include "RuntimePaths.h"

#include "raylib.h"

namespace {

std::string sLaunchDir;

std::string withTrailingSlash(std::string path) {
    if (!path.empty() && path.back() != '/' && path.back() != '\\') {
        path.push_back('/');
    }
    return path;
}

}  // namespace

namespace RuntimePaths {

void initialize() {
    const char* launchDirRaw = GetWorkingDirectory();
    if (launchDirRaw && launchDirRaw[0] != '\0') {
        sLaunchDir = withTrailingSlash(launchDirRaw);
    } else {
        sLaunchDir.clear();
    }

    const char* appDirRaw = GetApplicationDirectory();
    if (!appDirRaw || appDirRaw[0] == '\0') return;

    std::string appDir = withTrailingSlash(appDirRaw);
    std::string assetsDir = appDir + "assets";
    if (!DirectoryExists(assetsDir.c_str())) return;

    if (DirectoryExists("assets")) return;

    ChangeDirectory(appDir.c_str());
}

std::string persistencePath(const char* relativePath) {
    if (!relativePath || relativePath[0] == '\0') return sLaunchDir;
    if (sLaunchDir.empty()) return relativePath;
    return sLaunchDir + relativePath;
}

}  // namespace RuntimePaths

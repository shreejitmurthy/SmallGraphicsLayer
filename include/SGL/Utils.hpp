// Utilities

#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

namespace fs = std::filesystem;

#include "Math.hpp"
#include "Log.hpp"

namespace SmallGraphicsLayer::Utils {
fs::path FindPathUpwards(const fs::path& targetPath, fs::path startDir = fs::current_path()) {
    for (;;) {
        const fs::path candidate = startDir / targetPath;
        if (fs::exists(candidate)) {
            return fs::absolute(candidate);
        }
        if (startDir == startDir.root_path()) {
            Logger::Log()->error("[Utils::FindPathUpwards] Failed to locate: {}", targetPath);
            return {};
        }
        startDir = startDir.parent_path();
    }
}

std::string LoadFileIntoString(const std::string& filepath) {
    namespace fs = std::filesystem;

    fs::path foundPath = FindPathUpwards(filepath);

    if (foundPath.empty()) {
        return nullptr;
    }
    std::ifstream in(foundPath, std::ios::in | std::ios::binary);
    if (!in) {
        return nullptr;
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}
}  // namespace SmallGraphicsLayer
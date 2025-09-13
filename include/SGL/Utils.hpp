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
            Logger::Log()->error("Target {} not found", targetPath);
            return {};
        }
        startDir = startDir.parent_path();
    }
}

std::string LoadFileIntoString(const std::string& filepath) {
    namespace fs = std::filesystem;

    fs::path foundPath = FindPathUpwards(filepath);

    if (foundPath.empty()) {
        return std::string("[LoadFile] Failed to locate: ") + filepath;
    }
    std::ifstream in(foundPath, std::ios::in | std::ios::binary);
    if (!in) {
        return std::string("[LoadFile] Failed to open: ") + foundPath.string();
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}
}  // namespace SmallGraphicsLayer
// Utilities

#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

namespace fs = std::filesystem;

namespace SmallGraphicsLayer {
    namespace Utils {
        fs::path FindPathUpwards(const fs::path& targetPath, fs::path startDir) {
            for (;;) {
                const fs::path candidate = startDir / targetPath;
                if (fs::exists(candidate)) {
                    return fs::absolute(candidate);
                }
                if (startDir == startDir.root_path()) {
                    std::cout << "Target " << targetPath << " not found.\n";
                    return {};
                }
                startDir = startDir.parent_path();
            }
        }

        std::string LoadFileIntoString(const std::string& filepath) {
            namespace fs = std::filesystem;

            fs::path startDir = fs::current_path();
            fs::path foundPath = FindPathUpwards(filepath, startDir);

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

    }
};
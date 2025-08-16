#include "SGL/AssetManager.hpp"
#include "SGL/Utils.hpp"

SmallGraphicsLayer::File LoadFile(const std::string& filepath) {
    SmallGraphicsLayer::File out;
    out.data = SmallGraphicsLayer::Utils::LoadFileIntoString(filepath);
    return out;
}

void SmallGraphicsLayer::AssetManager::Request(const std::string& filepath, AssetType type) {
    switch (type) {
        case AssetType::File:
            files[filepath] = std::async(std::launch::async, LoadFile, filepath);
            break;

        default:
            std::cout << "Unsupported type, currently supports: File";
            break;
    }
}

template<>
SmallGraphicsLayer::File* SmallGraphicsLayer::AssetManager::Get<SmallGraphicsLayer::File>(const std::string& filepath) {
    if (loadedFiles.count(filepath)) return &loadedFiles[filepath];
    if (files.count(filepath) &&
        files[filepath].wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        loadedFiles[filepath] = files[filepath].get();
        files.erase(filepath);
        return &loadedFiles[filepath];
    }
    return nullptr;
}
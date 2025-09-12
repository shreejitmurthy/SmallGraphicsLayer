#include "SGL/AssetManager.hpp"
#include "SGL/Utils.hpp"

#include "stb_image.h"

void SmallGraphicsLayer::Texture::Free() {
    stbi_image_free(data);
}

SmallGraphicsLayer::File LoadFile(const std::string& filepath) {
    SmallGraphicsLayer::File out;
    out.data = SmallGraphicsLayer::Utils::LoadFileIntoString(filepath);
    return out;
}

SmallGraphicsLayer::Texture LoadTexture(const std::string& filepath) {
    SmallGraphicsLayer::Texture out;
    int w, h, channels;
    stbi_uc* pixels = stbi_load(SmallGraphicsLayer::Utils::FindPathUpwards(filepath).c_str(), &w, &h, &channels, 4);
    if (pixels != nullptr) {
        std::cout << "Loaded sprite at: " <<SmallGraphicsLayer::Utils::FindPathUpwards(filepath).c_str() << std::endl;
    }
    out.data = pixels;
    out.width = w;
    out.height = h;
    return out;
}

void SmallGraphicsLayer::AssetManager::Request(const std::string& filepath, AssetType type) {
    switch (type) {
        case AssetType::File:
            files[filepath] = std::async(std::launch::async, LoadFile, filepath);
            break;
        
        case AssetType::Texture:
            textures[filepath] = std::async(std::launch::async, LoadTexture, filepath);
            break;

        default:
            std::cout << "Unsupported type, currently supports: File";
            break;
    }
}

template<>
SmallGraphicsLayer::File* SmallGraphicsLayer::AssetManager::Get<SmallGraphicsLayer::File>(const std::string& filepath) {
    return GetFile(filepath);
}

template<>
SmallGraphicsLayer::Texture* SmallGraphicsLayer::AssetManager::Get<SmallGraphicsLayer::Texture>(const std::string& filepath) {
    return GetTexture(filepath);
}

SmallGraphicsLayer::File* SmallGraphicsLayer::AssetManager::GetFile(const std::string& filepath) {
    if (loadedFiles.count(filepath)) return &loadedFiles[filepath];
    if (files.count(filepath) &&
        files[filepath].wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        loadedFiles[filepath] = files[filepath].get();
        files.erase(filepath);
        return &loadedFiles[filepath];
    }
    return nullptr;
}

SmallGraphicsLayer::Texture* SmallGraphicsLayer::AssetManager::GetTexture(const std::string& path) {
    if (loadedTextures.count(path)) return &loadedTextures[path];
    auto it = textures.find(path);
    if (it == textures.end()) return nullptr;  // not requested
    loadedTextures[path] = it->second.get();   // blocks until ready
    textures.erase(it);
    return &loadedTextures[path];
}

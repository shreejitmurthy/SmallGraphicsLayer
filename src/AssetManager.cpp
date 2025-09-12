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


// custom deleter that frees the stb buffer then deletes the Texture
struct TextureDeleter {
    void operator()(SmallGraphicsLayer::Texture* t) const noexcept {
        if (!t) return;
        if (t->data) {
            stbi_image_free(t->data);
            t->data = nullptr; // prevent accidental double free if someone *does* call Free()
        }
        delete t;
    }
};

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

static std::shared_ptr<SmallGraphicsLayer::Texture> make_managed_texture(SmallGraphicsLayer::Texture&& src) {
    // move/copy the plain POD Texture into heap and manage it with a custom deleter.
    auto heapTex = new SmallGraphicsLayer::Texture(src);   // copies width/height/pointer
    return std::shared_ptr<SmallGraphicsLayer::Texture>(heapTex, TextureDeleter{});
}

SmallGraphicsLayer::Texture* SmallGraphicsLayer::AssetManager::GetTexture(const std::string& path) {
    // check if already cached
    if (auto it = loadedTextures.find(path); it != loadedTextures.end()) {
        return it->second.get();
    }

    // Was it requested?
    auto pit = textures.find(path);
    if (pit == textures.end()) {
        return nullptr;  // not requested
    }

    Texture decoded = pit->second.get();
    textures.erase(pit);

    // wrap in shared_ptr with deleter so pixels are freed automatically
    auto handle = make_managed_texture(std::move(decoded));
    Texture* raw = handle.get();
    loadedTextures.emplace(path, std::move(handle));

    return raw;
}


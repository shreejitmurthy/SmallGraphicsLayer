#include "SGL/AssetManager.hpp"
#include "SGL/Utils.hpp"

#include "stb_image.h"

std::unordered_map<std::string, std::future<SmallGraphicsLayer::File>> SmallGraphicsLayer::AssetManager::s_files{};
std::unordered_map<std::string, SmallGraphicsLayer::File> SmallGraphicsLayer::AssetManager::s_loadedFiles{};
std::unordered_map<std::string, std::future<SmallGraphicsLayer::Texture>> SmallGraphicsLayer::AssetManager::s_textures{};
std::unordered_map<std::string, std::shared_ptr<SmallGraphicsLayer::Texture>> SmallGraphicsLayer::AssetManager::s_loadedTextures{};

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
            s_files[filepath] = std::async(std::launch::async, LoadFile, filepath);
            break;
        
        case AssetType::Texture:
            s_textures[filepath] = std::async(std::launch::async, LoadTexture, filepath);
            break;

        default:
            std::cout << "Unsupported type, currently supports: File";
            break;
    }
}

SmallGraphicsLayer::File* SmallGraphicsLayer::AssetManager::GetFile(const std::string& filepath) {
    if (s_loadedFiles.count(filepath)) return &s_loadedFiles[filepath];
    if (s_files.count(filepath) &&
        s_files[filepath].wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        s_loadedFiles[filepath] = s_files[filepath].get();
        s_files.erase(filepath);
        return &s_loadedFiles[filepath];
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
    if (auto it = s_loadedTextures.find(path); it != s_loadedTextures.end()) {
        return it->second.get();
    }

    // Was it requested?
    auto pit = s_textures.find(path);
    if (pit == s_textures.end()) {
        return nullptr;  // not requested
    }

    Texture decoded = pit->second.get();
    s_textures.erase(pit);

    // wrap in shared_ptr with deleter so pixels are freed automatically
    auto handle = make_managed_texture(std::move(decoded));
    Texture* raw = handle.get();
    s_loadedTextures.emplace(path, std::move(handle));

    return raw;
}


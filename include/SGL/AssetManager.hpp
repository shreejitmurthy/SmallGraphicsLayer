#pragma once

#include <unordered_map>
#include <string>
#include <future>
#include <tuple>

namespace SmallGraphicsLayer {
enum class AssetType {
    File,     // use normal c++ file i/o
    Texture,  // use stb_image
    Model,    // use assimp (or whatever i plan to use)
    Sound     // use something idk
};

typedef struct File { std::string data; } File;

typedef std::tuple<int, int, unsigned char*> TextureData;

typedef struct Texture { 
    int width, height;
    unsigned char* pixels;

    TextureData GetData() {
        return {width, height, pixels};
    }

    void Free();
    
} Texture;

class AssetManager {
public:
    static void Request(const std::string& filepath, AssetType type = AssetType::File);

    // enforced getters are better than template getter
    static File* GetFile(const std::string& filepath);
    static Texture* GetTexture(const std::string& path);
private:
    static std::unordered_map<std::string, std::future<File>> s_files;
    static std::unordered_map<std::string, File> s_loadedFiles;
    
    static std::unordered_map<std::string, std::future<Texture>> s_textures;
    static std::unordered_map<std::string, std::shared_ptr<Texture>> s_loadedTextures;
};
}  // namespace SmallGraphicsLayer
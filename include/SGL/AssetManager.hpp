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
    unsigned char* data;

    TextureData GetData() {
        return {width, height, data};
    }

    void Free();
    
} Texture;

class AssetManager {
public:
    void Request(const std::string& filepath, AssetType type = AssetType::File);
    
    // template getter, but i honestly much prefer the enforced getters
    template<typename T>
    T* Get(const std::string& filepath);

    File* GetFile(const std::string& filepath);
    Texture* GetTexture(const std::string& path);
private:
    std::unordered_map<std::string, std::future<File>> files;
    std::unordered_map<std::string, std::future<Texture>> textures;
    std::unordered_map<std::string, File> loadedFiles;
    // std::unordered_map<std::string, Texture> loadedTextures;

    std::unordered_map<std::string, std::shared_ptr<Texture>> loadedTextures;
};
}
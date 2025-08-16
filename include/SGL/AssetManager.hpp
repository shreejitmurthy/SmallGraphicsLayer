#pragma once

#include <unordered_map>
#include <string>
#include <future>

namespace SmallGraphicsLayer {
    enum class AssetType {
        File,     // use normal c++ file i/o
        Texture,  // use stb_image
        Model,    // use assimp (or whatever i plan to use)
        Sound     // use something idk
    };

    typedef struct File { std::string data; } File;

    class AssetManager {
    public:
        void Request(const std::string& filepath, AssetType type = AssetType::File);
        
        template<typename T>
        T* Get(const std::string& filepath);
    private:
        std::unordered_map<std::string, std::future<File>> files;
        std::unordered_map<std::string, File> loadedFiles;
    };
}
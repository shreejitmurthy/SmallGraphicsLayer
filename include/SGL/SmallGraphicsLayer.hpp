#pragma once

#include <sokol/sokol_gfx.h>

#include <cstdint>
#include <future>

typedef sg_color SGLColour;

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

    class Device {
    public:
        void Init(/* SGLFlags flags, */ int width = 0, int height = 0);
        void Clear(SGLColour clear_col = {0.15f, 0.15f, 0.2f, 1.0f});
        void Render();
        void Shutdown();
    private:
        sg_pass_action pass_action;
        sg_swapchain swapchain;
    };
};

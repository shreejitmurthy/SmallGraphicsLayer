#include "SGL/SmallGraphicsLayer.hpp"
#include "SGL/Utils.hpp"

#if defined(WINDOW_SAPP) && defined(__APPLE__) && defined(__MACH__)
    #include <sokol/sokol_app.h>
    #include <sokol/sokol_glue.h>
    #include <sokol/sokol_log.h>
#endif

#include <sokol/sokol_gfx.h>

void SmallGraphicsLayer::Device::Init(int width, int height) {
    sg_desc desc{};

    #if defined(WINDOW_SAPP)
        swapchain = sglue_swapchain();
        desc.environment = sglue_environment();
        desc.logger.func = slog_func;
    #elif defined(WINDOW_SDL)
        swapchain = {};
        swapchain.width = width;
        swapchain.height = height;
        swapchain.color_format = SG_PIXELFORMAT_RGBA8;
    // TODO: uncomment this, ts *was* pmo
    // #else
    //     #error "No window backend defined! Define WINDOW_SAPP or WINDOW_SDL."
    #endif

        sg_setup(&desc);

        pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
}

void SmallGraphicsLayer::Device::Clear(SGLColour clear_col) {
    pass_action.colors[0].clear_value = clear_col;
}

void SmallGraphicsLayer::Device::Render() {
    sg_begin_pass((sg_pass){
        .action    = pass_action,
        .swapchain = swapchain
    });
    sg_end_pass();
    sg_commit();
}

void SmallGraphicsLayer::Device::Shutdown() {
    sg_shutdown();
}

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
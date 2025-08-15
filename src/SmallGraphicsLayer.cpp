#include "SGL/SmallGraphicsLayer.hpp"

#include <iostream>

#if defined(WINDOW_SAPP)
    #define SOKOL_GLUE_IMPL
    #define SOKOL_LOG_IMPL
    #include <sokol/sokol_glue.h>
    #include <sokol/sokol_log.h>
#endif

#define SOKOL_GFX_IMPL
#define SOKOL_GLCORE
#include <sokol/sokol_gfx.h>

#include "SGL/SmallGraphicsLayer.hpp"

void HelloSGL() {
    std::cout << "Hello, SGL!" << std::endl;
}

void SimpleGraphicsLayer::Init(/* SGLFlags flags, */ int width, int height) {
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

void SimpleGraphicsLayer::Clear(SGLColour clear_col) {
    pass_action.colors[0].clear_value = clear_col;
}

void SimpleGraphicsLayer::Render() {
    sg_begin_pass((sg_pass){
        .action = pass_action,
        .swapchain = swapchain
    });
    sg_end_pass();
    sg_commit();
}

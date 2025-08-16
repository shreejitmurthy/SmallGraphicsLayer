#include "SGL/SmallGraphicsLayer.hpp"

#include <iostream>

#if defined(WINDOW_SAPP) && defined(__APPLE__) && defined(__MACH__)
    #include <sokol/sokol_app.h>
    #include <sokol/sokol_glue.h>
    #include <sokol/sokol_log.h>
#endif

#include <sokol/sokol_gfx.h>

void SmallGraphicsLayer::Init(int width, int height) {
    sg_desc desc{};

    #if defined(WINDOW_SAPP)
        SGLState.swapchain = sglue_swapchain();
        desc.environment = sglue_environment();
        desc.logger.func = slog_func;
    #elif defined(WINDOW_SDL)
        SGLState.swapchain = {};
        SGLState.swapchain.width = width;
        SGLState.swapchain.height = height;
        SGLState.swapchain.color_format = SG_PIXELFORMAT_RGBA8;
    // TODO: uncomment this, ts *was* pmo
    // #else
    //     #error "No window backend defined! Define WINDOW_SAPP or WINDOW_SDL."
    #endif

        sg_setup(&desc);

        SGLState.pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
}

void SmallGraphicsLayer::Clear(SGLColour clear_col) {
    SGLState.pass_action.colors[0].clear_value = clear_col;
}

void SmallGraphicsLayer::Render() {
    sg_begin_pass((sg_pass){
        .action    = SGLState.pass_action,
        .swapchain = SGLState.swapchain
    });
    sg_end_pass();
    sg_commit();
}
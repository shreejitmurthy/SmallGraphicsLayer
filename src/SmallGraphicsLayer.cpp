#include "SGL/SmallGraphicsLayer.hpp"

#if defined(WINDOW_SAPP) && defined(__APPLE__) && defined(__MACH__)
    #include <sokol/sokol_app.h>
    #include <sokol/sokol_glue.h>
    #include <sokol/sokol_log.h>
#endif

#include <array>

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

void SmallGraphicsLayer::Device::Clear(Colour clear_col) {
    pass_action.colors[0].clear_value = clear_col;

    sg_begin_pass((sg_pass){
        .action    = pass_action,
        .swapchain = swapchain
    });
}

void SmallGraphicsLayer::Device::Refresh() {
    sg_end_pass();
    sg_commit();
}

void SmallGraphicsLayer::Device::Shutdown() {
    sg_shutdown();
}

SmallGraphicsLayer::AttributeBuilder& SmallGraphicsLayer::AttributeBuilder::Begin(Primitives primative) {
    elements = static_cast<int>(primative);

    shd = sg_make_shader(attributes_main_shader_desc(sg_query_backend()));

    // not cross-platform safe
    pipeline = sg_make_pipeline((sg_pipeline_desc){
        .shader = shd,
        .layout = {
            .attrs = {
                [ATTR_attributes_main_position].format = SG_VERTEXFORMAT_FLOAT3,
                [ATTR_attributes_main_aColor].format = SG_VERTEXFORMAT_FLOAT4
            }
        },
    });

    return *this;
}

// perhaps a bool that asks for pixel to NDC for help? default to true.
SmallGraphicsLayer::AttributeBuilder& SmallGraphicsLayer::AttributeBuilder::Vertex(Position pos, Colour col) {
    std::array<float, 7> chunk = {pos.x, pos.y, pos.z,  col.r, col.g, col.b, col.a};
    vertices.insert(vertices.end(), chunk.begin(), chunk.end());
    // should personally throw an error if too many chunks are inserted
    // should personally throw a warn if there are less than expected as well
    return *this;
}

void SmallGraphicsLayer::AttributeBuilder::End() {
    // not cross-platform safe
    bindings.vertex_buffers[0] = sg_make_buffer((sg_buffer_desc){
        .size = vertices.size() * sizeof(float),
        .data = {.ptr = vertices.data(), .size = vertices.size() * sizeof(float)},
    });
}

void SmallGraphicsLayer::AttributeBuilder::Draw() {
    sg_apply_pipeline(pipeline);
    sg_apply_bindings(&bindings);
    sg_draw(0, elements, 1);
}

void SmallGraphicsLayer::AttributeBuilder::Destroy() {
    sg_destroy_shader(shd);
    sg_destroy_buffer(vbuf);
    sg_destroy_buffer(ibuf);
    sg_destroy_pipeline(pipeline);
}

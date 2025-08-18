#include "SGL/SmallGraphicsLayer.hpp"

#if defined(WINDOW_SAPP) && defined(__APPLE__) && defined(__MACH__)
    #include <sokol/sokol_app.h>
    #include <sokol/sokol_glue.h>
    #include <sokol/sokol_log.h>
#endif

#include <array>

#include <sokol/sokol_gfx.h>

void SmallGraphicsLayer::Device::Init(int w, int h) {
    sg_desc desc{};

    width = w;
    height = h;

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

SmallGraphicsLayer::AttributeBuilder& SmallGraphicsLayer::AttributeBuilder::Begin(Primitives primitive) {
    elements = static_cast<int>(primitive);

    shd = sg_make_shader(attributes_main_shader_desc(sg_query_backend()));

    // not cross-platform safe
    pipeline = sg_make_pipeline((sg_pipeline_desc){
        .shader = shd,
        .index_type = elements == 4 ? SG_INDEXTYPE_UINT16 : SG_INDEXTYPE_NONE,
        .layout = {
            .attrs = {
                [ATTR_attributes_main_position].format = SG_VERTEXFORMAT_FLOAT3,
                [ATTR_attributes_main_colour0].format = SG_VERTEXFORMAT_FLOAT4
            }
        },
    });

    // 4 + 2 indices
    if (primitive == Primitives::Quad) elements += 2;
    expected_chunks = elements;
    chunks = 0;

    return *this;
}

// should personally throw a warn if there are less than expected chunks as well
SmallGraphicsLayer::AttributeBuilder& SmallGraphicsLayer::AttributeBuilder::Vertex(Position pos, Colour col, bool useNDC) {
    enable_ndc = useNDC;  
    Math::Vec3 position = enable_ndc ? pos : _Pixels2NDC(pos);
    std::array<float, 7> chunk = {position.x, position.y, position.z,  col.r, col.g, col.b, col.a};
    vertices.insert(vertices.end(), chunk.begin(), chunk.end());
    chunks++;
    return *this;
}

SmallGraphicsLayer::AttributeBuilder& SmallGraphicsLayer::AttributeBuilder::Index(SmallGraphicsLayer::Index index) {
    std::array<std::uint16_t, 3> chunk = {index.x, index.y, index.z};
    indices.insert(indices.end(), chunk.begin(), chunk.end());
    chunks++;
    return *this;
}

void SmallGraphicsLayer::AttributeBuilder::End() {
    // not cross-platform safe
    if (vertices.size() > 0) {
        bindings.vertex_buffers[0] = sg_make_buffer((sg_buffer_desc){
            .size = vertices.size() * sizeof(float),
            .data = {.ptr = vertices.data(), .size = vertices.size() * sizeof(float)},
        });
    }

    if (indices.size() > 0) {
        bindings.index_buffer = sg_make_buffer((sg_buffer_desc){
            .usage.index_buffer = true,
            .data = {.ptr = indices.data(), .size = indices.size() * sizeof(std::uint16_t)},
        });
    }
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

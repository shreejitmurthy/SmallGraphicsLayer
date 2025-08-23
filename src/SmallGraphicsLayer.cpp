#include "SGL/SmallGraphicsLayer.hpp"
#include "SGL/Utils.hpp"

#if defined(WINDOW_SAPP) && defined(__APPLE__) && defined(__MACH__)
    #include <sokol/sokol_app.h>
    #include <sokol/sokol_glue.h>
    #include <sokol/sokol_log.h>
#endif

#include <array>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
        #if defined(__APPLE__)
            swapchain.color_format = SG_PIXELFORMAT_BGRA8;
        #else
            swapchain.color_format = SG_PIXELFORMAT_RGBA8;
        #endif
        swapchain.sample_count = 1;
        // swapchain.depth_format = SG_PIXELFORMAT_DEPTH;
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

    shader = sg_make_shader(attributes_main_shader_desc(sg_query_backend()));

    // not cross-platform safe
    sg_pipeline_desc pip_desc = {};
    pip_desc.shader = shader;
    pip_desc.index_type = elements == 4 ? SG_INDEXTYPE_UINT16 : SG_INDEXTYPE_NONE;
    pip_desc.layout.attrs[ATTR_attributes_main_position].format = SG_VERTEXFORMAT_FLOAT3;
    pip_desc.layout.attrs[ATTR_attributes_main_colour0].format = SG_VERTEXFORMAT_FLOAT4;
    pipeline = sg_make_pipeline(&pip_desc);

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
    if (vertices.size() > 0) {
        sg_buffer_desc vbuf_desc = {};
        vbuf_desc.size = vertices.size() * sizeof(float);
        vbuf_desc.data.ptr = vertices.data();
        vbuf_desc.data.size = vertices.size() * sizeof(float);
        bindings.vertex_buffers[0] = sg_make_buffer(&vbuf_desc);
    }

    if (indices.size() > 0) {
        sg_buffer_desc ibuf_desc = {};
        ibuf_desc.usage.index_buffer = true;
        ibuf_desc.data.ptr = indices.data();
        ibuf_desc.size = indices.size() * sizeof(std::uint16_t);
        bindings.index_buffer = sg_make_buffer(&ibuf_desc);
    }
}

void SmallGraphicsLayer::AttributeBuilder::Draw() {
    sg_apply_pipeline(pipeline);
    sg_apply_bindings(&bindings);
    sg_draw(0, elements, 1);
}

void SmallGraphicsLayer::AttributeBuilder::Destroy() {
    sg_destroy_shader(shader);
    sg_destroy_buffer(vbuf);
    sg_destroy_buffer(ibuf);
    sg_destroy_pipeline(pipeline);
}

SmallGraphicsLayer::Sprite::Sprite(const std::string& path) {
    int w, h, channels;
    stbi_uc* pixels = stbi_load(Utils::FindPathUpwards(path).c_str(), &w, &h, &channels, 4);
    if (pixels != nullptr) {
        std::cout << "Loaded sprite at: " << Utils::FindPathUpwards(path).c_str() << std::endl;
    }

    size = {static_cast<float>(w), static_cast<float>(h)};

    sg_image_desc img_desc = {};
    img_desc.width = w;
    img_desc.height = h;
    img_desc.sample_count = 1;
    img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    img_desc.data.subimage[0][0].ptr = pixels;
    img_desc.data.subimage[0][0].size = static_cast<std::size_t>(w * h * 4);
    image = sg_make_image(img_desc);

    sg_sampler_desc smp_desc = {};
    smp_desc.min_filter = SG_FILTER_LINEAR;
    smp_desc.mag_filter = SG_FILTER_NEAREST;
    sg_sampler smp = sg_make_sampler(smp_desc);

    const float vertices[] = {
        0.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f,   1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,   0.0f, 1.0f
    };

    sg_buffer_desc vbuf_desc = {};
    vbuf_desc.data = SG_RANGE(vertices);
    vbuf_desc.usage.vertex_buffer = true;
    vbuf = sg_make_buffer(vbuf_desc);

    const std::uint16_t indices[] = { 0, 1, 2,  2, 3, 0 };
    sg_buffer_desc ibuf_desc = {};
    ibuf_desc.data = SG_RANGE(indices);
    ibuf_desc.usage.index_buffer = true;
    ibuf = sg_make_buffer(ibuf_desc);

    stbi_image_free(pixels);

    sg_shader shd = sg_make_shader(sprite_main_shader_desc(sg_query_backend()));

    sg_pipeline_desc pip_desc = {};
    pip_desc.shader = shd;
    pip_desc.layout.attrs[ATTR_sprite_main_pos].format = SG_VERTEXFORMAT_FLOAT4;
    pip_desc.layout.attrs[ATTR_sprite_main_texcoord0].format = SG_VERTEXFORMAT_FLOAT2;
    pip_desc.sample_count = 1;
    pip_desc.color_count = 1;
    #if defined(__APPLE__)
        pip_desc.colors->pixel_format = SG_PIXELFORMAT_BGRA8;
    #else
        pip_desc.colors->pixel_format = SG_PIXELFORMAT_RGBA8;
    #endif
    pip_desc.colors->blend.enabled = true;
    pip_desc.colors->blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    pip_desc.colors->blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip_desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH_STENCIL;
    pip_desc.index_type = SG_INDEXTYPE_UINT16;
    pipeline = sg_make_pipeline(pip_desc);

    bindings.vertex_buffers[0] = vbuf;
    bindings.index_buffer = ibuf;
    bindings.images[IMG_sprite_tex] = image;
    bindings.samplers[SMP_sprite_smp] = smp;

}

void SmallGraphicsLayer::Sprite::Draw(Math::Vec2 position, Math::Vec2 origin, Math::Vec2 scale) {
    params.mvp = Math::Mat4::ortho(0.0f, 800, 600, 0.0f, -1.0f, 1.0f);

    params.mvp *= Math::Mat4::translate({ position.x - origin.x, position.y - origin.y, 0.0f });
    params.mvp *= Math::Mat4::scale({ size.x * scale.x, size.y * scale.y, 1.0f });

    sg_apply_pipeline(pipeline);
    sg_apply_bindings(&bindings);
    sg_apply_uniforms(UB_sprite_params, SG_RANGE(params));
    sg_draw(0, 6, 1);
}

void SmallGraphicsLayer::Sprite::Destroy() {
    sg_destroy_buffer(vbuf);
    sg_destroy_buffer(ibuf);
    sg_destroy_image(image);
    sg_destroy_pipeline(pipeline);
}

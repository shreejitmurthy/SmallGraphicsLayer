#include "SGL/SmallGraphicsLayer.hpp"
// #include "SGL/Utils.hpp"  // was causing duplicate symbol issues
#include "SGL/Log.hpp"

#if defined(WINDOW_SAPP) && defined(__APPLE__) && defined(__MACH__)
    #include <sokol_app.h>
    #include <sokol_glue.h>
#endif

#include <array>

// TODO: Apply this everywhere where needed
// Currently only used in Instanced Renderer
inline sg_buffer make_ibuf() {
    const std::uint16_t indices[] = { 0, 1, 2,  2, 3, 0 };
    sg_buffer_desc ibuf_desc = {};
    ibuf_desc.data = SG_RANGE(indices);
    ibuf_desc.usage.index_buffer = true;
    ibuf_desc.label = "indices";
    return sg_make_buffer(&ibuf_desc);;
}

inline sg_buffer make_unit_vbuf() {
    const float vertices[] = {
        0.0f, 0.0f,   0.0f, 0.0f,
        1.0f, 0.0f,   1.0f, 0.0f,
        1.0f, 1.0f,   1.0f, 1.0f,
        0.0f, 1.0f,   0.0f, 1.0f
    };

    sg_buffer_desc vbuf_desc = {};
    vbuf_desc.data = SG_RANGE(vertices);
    vbuf_desc.usage.vertex_buffer = true;
    // change if needed
    vbuf_desc.usage.immutable = true;
    vbuf_desc.label = "vertices";
    return sg_make_buffer(&vbuf_desc);;
}

inline void set_alpha_blend(sg_pipeline_desc& pip_desc) {
    pip_desc.colors[0].blend.enabled          = true;
    pip_desc.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
    pip_desc.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip_desc.colors[0].blend.op_rgb           = SG_BLENDOP_ADD;
    pip_desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pip_desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip_desc.colors[0].blend.op_alpha         = SG_BLENDOP_ADD;
}

using namespace SmallGraphicsLayer;

void SmallGraphicsLayer::EnableLogger() {
    Logger::Init(true);
    Logger::Log()->info("Enabled non-error/critical logging");
}

std::uint32_t Device::width  = 0;
std::uint32_t Device::height = 0;

void Device::Init(int w, int h) {
    if (!Logger::isEnabled()) Logger::Init();

    sg_desc desc{};

    width = w;
    height = h;

    std::string backend = "";
    
    #if defined(WINDOW_SAPP)
        Logger::Log()->info("Window Backend: sokol_app");
        swapchain = sglue_swapchain();
        desc.environment = sglue_environment();
    #elif defined(WINDOW_SDL)
        Logger::Log()->info("Window Backend: SDL");
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
    // // TODO: uncomment this, ts *was* pmo
    // #else
    //     #error "No window backend defined! Define WINDOW_SAPP or WINDOW_SDL."
    #endif

    switch (sg_query_backend()) {
        case SG_BACKEND_GLCORE:
            backend = "OpenGL";
            break;
        case SG_BACKEND_D3D11:
            backend = "D3D11";
            break;
        case SG_BACKEND_METAL_MACOS:
        case SG_BACKEND_METAL_IOS:
        case SG_BACKEND_METAL_SIMULATOR:
            backend = "Metal";
            break;
        case SG_BACKEND_WGPU:
            backend = "WebGPU";
            break;
        default:
            backend = "Other";
    }

    Logger::Log()->info("Graphics Backend: {}", backend);

    sg_setup(&desc);

    pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
}

void Device::Clear(Colour clear_col) {
    pass_action.colors[0].clear_value = clear_col;

    sg_pass pass = {};
    pass.action = pass_action;
    pass.swapchain = swapchain;

    sg_begin_pass(&pass);
}

void Device::Refresh() {
    sg_end_pass();
    sg_commit();
}

void Device::Shutdown() {
    sg_shutdown();
}

AttributeProgram::AttributeProgram(const std::string& frag) {
    if (!frag.empty()) {
        frag_src_storage = frag;
        applied_uniforms = false;
        desc.vertex_func.source = 
            "#version 410\n"
            "layout(location=0) in vec3 position;\n"
            "layout(location=1) in vec4 colour0;\n"
            "out vec2 TexCoord;\n"
            "void main() {\n"
            "    gl_Position = vec4(position.xyz, 1.0);\n"
            "    TexCoord = vec2(colour0.x, colour0.y);\n"
            "}";
        desc.fragment_func.source = frag_src_storage.c_str();
        desc.uniform_blocks[0].stage = SG_SHADERSTAGE_FRAGMENT;
        desc.uniform_blocks[0].size  = sizeof(fs_params);

        desc.uniform_blocks[0].glsl_uniforms[0].glsl_name = "iResolution";
        desc.uniform_blocks[0].glsl_uniforms[0].type = SG_UNIFORMTYPE_FLOAT2;

        desc.uniform_blocks[0].glsl_uniforms[1].glsl_name = "iTime";
        desc.uniform_blocks[0].glsl_uniforms[1].type = SG_UNIFORMTYPE_FLOAT;

        params.resolution[0] = 0;
        params.resolution[1] = 0;
        params.time = 0;
    }
}

void AttributeProgram::ApplyDefaultUniforms(Math::Vec2 resolution, float time) {
    params.resolution[0] = resolution.x;
    params.resolution[1] = resolution.y;
    params.time = time;
    applied_uniforms = true;
}

AttributeBuilder &AttributeBuilder::Begin(Primitives primitive) {
    elements = static_cast<int>(primitive);

    if (use_custom_fragment) {
        shader = sg_make_shader(program.GetDesc());
        sg_pipeline_desc pip_desc = {};
        pip_desc.shader = shader;
        pip_desc.index_type = elements == 4 ? SG_INDEXTYPE_UINT16 : SG_INDEXTYPE_NONE;
        pip_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
        pip_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT4;
        pipeline = sg_make_pipeline(&pip_desc);
    } else {
        shader = sg_make_shader(attributes_main_shader_desc(sg_query_backend()));

        sg_pipeline_desc pip_desc = {};
        pip_desc.shader = shader;
        pip_desc.index_type = elements == 4 ? SG_INDEXTYPE_UINT16 : SG_INDEXTYPE_NONE;
        pip_desc.layout.attrs[ATTR_attributes_main_position].format = SG_VERTEXFORMAT_FLOAT3;
        pip_desc.layout.attrs[ATTR_attributes_main_colour0].format = SG_VERTEXFORMAT_FLOAT4;
        pipeline = sg_make_pipeline(&pip_desc);
    }

    // 4 + 2 indices
    if (primitive == Primitives::Quad) elements += 2;
    expected_chunks = elements;
    chunks = 0;

    return *this;
}

// should personally throw a warn if there are less than expected chunks as well
AttributeBuilder& AttributeBuilder::Vertex(Position pos, Colour col, bool useNDC) {
    enable_ndc = useNDC;  
    Math::Vec3 position = enable_ndc ? pos : _Pixels2NDC(pos);
    std::array<float, 7> chunk = {position.x, position.y, position.z,  col.r, col.g, col.b, col.a};
    vertices.insert(vertices.end(), chunk.begin(), chunk.end());
    chunks++;
    return *this;
}

AttributeBuilder& AttributeBuilder::Index(SmallGraphicsLayer::Index index) {
    std::array<std::uint16_t, 3> chunk = {index.x, index.y, index.z};
    indices.insert(indices.end(), chunk.begin(), chunk.end());
    chunks++;
    return *this;
}

void AttributeBuilder::End() {
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

void AttributeBuilder::Draw() {
    sg_apply_pipeline(pipeline);
    sg_apply_bindings(&bindings);
    sg_draw(0, elements, 1);
}

void AttributeBuilder::Draw(AttributeProgram p) {
    sg_apply_pipeline(pipeline);
    sg_apply_bindings(&bindings);
        if (use_custom_fragment) {
        if (!p.HasAppliedUniforms()) p.ApplyDefaultUniforms();
        auto params = p.GetUniformParams();
        sg_apply_uniforms(0, SG_RANGE(params));
    }
    sg_draw(0, elements, 1);
}

void AttributeBuilder::Destroy() {
    sg_destroy_shader(shader);
    sg_destroy_buffer(vbuf);
    sg_destroy_buffer(ibuf);
    sg_destroy_pipeline(pipeline);
}

Sprite::Sprite(std::tuple<int, int, unsigned char*> data) {
    int w = std::get<0>(data), h = std::get<1>(data);
    unsigned char* pixels = std::get<2>(data);

    size = {static_cast<float>(w), static_cast<float>(h)};

    sg_image_desc img_desc = {};
    img_desc.width = w;
    img_desc.height = h;
    img_desc.sample_count = 1;
    img_desc.data.mip_levels[0].ptr = pixels;
    img_desc.data.mip_levels[0].size = static_cast<std::size_t>(w * h * 4);
    image = sg_make_image(img_desc);

    sg_sampler_desc smp_desc = {};
    smp_desc.min_filter = SG_FILTER_LINEAR;
    smp_desc.mag_filter = SG_FILTER_NEAREST;
    sg_sampler smp = sg_make_sampler(smp_desc);

    const float vertices[] = {
        0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,   0.0f, 1.0f
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

    sg_shader shd = sg_make_shader(sprite_main_shader_desc(sg_query_backend()));

    sg_pipeline_desc pip_desc = {};
    pip_desc.shader = shd;
    pip_desc.layout.attrs[ATTR_sprite_main_pos].format = SG_VERTEXFORMAT_FLOAT3;
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
    sg_view_desc view_desc = {};
    view_desc.texture.image = image;
    bindings.views[VIEW_sprite_tex] = sg_make_view(&view_desc);
    bindings.samplers[SMP_sprite_smp] = smp;
}

void Sprite::Update(Math::Vec2 position, Math::Vec2 origin, Math::Vec2 scale) {
    params.mvp = GetDefaultProjection();
    params.mvp *= Math::Mat4::translate({ position.x - origin.x, position.y - origin.y, 0.0f });
    params.mvp *= Math::Mat4::scale({ size.x * scale.x, size.y * scale.y, 1.0f });
}

void Sprite::Draw() {
    sg_apply_pipeline(pipeline);
    sg_apply_bindings(&bindings);
    sg_apply_uniforms(UB_sprite_params, SG_RANGE(params));
    sg_draw(0, 6, 1);
}

void Sprite::Destroy() {
    sg_destroy_buffer(vbuf);
    sg_destroy_buffer(ibuf);
    sg_destroy_buffer(bindings.vertex_buffers[0]);
    sg_destroy_buffer(bindings.index_buffer);
    sg_destroy_view(bindings.views[VIEW_sprite_tex]);
    sg_destroy_sampler(bindings.samplers[SMP_sprite_smp]);
    sg_destroy_image(image);
    sg_destroy_pipeline(pipeline);
}

InstancedSprite::InstancedSprite(std::tuple<int, int, unsigned char*> data, Math::Vec2 tileSize, std::uint16_t maxInstances) {
    tile_size = tileSize;
    vs_params.mvp = GetDefaultProjection();

    sg_shader shader = sg_make_shader(instance_main_shader_desc(sg_query_backend()));
    int w = std::get<0>(data), h = std::get<1>(data);
    unsigned char* pixels = std::get<2>(data);
    sg_image_desc image_desc = {};
    image_desc.width = w;
    image_desc.height = h;
    image_desc.data.mip_levels[0].ptr = pixels;
    image_desc.data.mip_levels[0].size = static_cast<std::size_t>(w * h * 4);
    sg_image image = sg_make_image(image_desc);

    this->w = w;
    this->h = h;

    sg_sampler_desc smp_desc = {};
    smp_desc.min_filter = SG_FILTER_LINEAR;
    smp_desc.mag_filter = SG_FILTER_NEAREST;
    sg_sampler smp = sg_make_sampler(smp_desc);

    bindings.vertex_buffers[0] = make_unit_vbuf();
    bindings.index_buffer = make_ibuf();

    sg_view_desc view_desc = {};
    view_desc.texture.image = image;

    bindings.views[VIEW_instance_tex] = sg_make_view(&view_desc);
    bindings.samplers[SMP_instance_smp] = smp;

    sg_buffer_desc inst_desc = {};
    inst_desc.size = sizeof(InstanceData) * maxInstances;
    inst_desc.usage.stream_update = true;
    inst_desc.usage.vertex_buffer = true;
    inst_desc.label = "instance-buffer";
    bindings.vertex_buffers[1] = sg_make_buffer(inst_desc);

    sg_pipeline_desc pip_desc = {};
    pip_desc.shader = shader;
    pip_desc.index_type = SG_INDEXTYPE_UINT16;

    pip_desc.layout.buffers[0].stride    = 4 * sizeof(float);
    pip_desc.layout.buffers[1].step_func = SG_VERTEXSTEP_PER_INSTANCE;

    pip_desc.layout.attrs[ATTR_instance_main_aPos].format       = SG_VERTEXFORMAT_FLOAT2;
    pip_desc.layout.attrs[ATTR_instance_main_aPos].buffer_index = 0;
    pip_desc.layout.attrs[ATTR_instance_main_aUV].format        = SG_VERTEXFORMAT_FLOAT2;
    pip_desc.layout.attrs[ATTR_instance_main_aUV].buffer_index  = 0;

    pip_desc.layout.attrs[ATTR_instance_main_aOffset].format           = SG_VERTEXFORMAT_FLOAT2;
    pip_desc.layout.attrs[ATTR_instance_main_aOffset].buffer_index     = 1;
    pip_desc.layout.attrs[ATTR_instance_main_aUVOffset].format         = SG_VERTEXFORMAT_FLOAT2;
    pip_desc.layout.attrs[ATTR_instance_main_aUVOffset].buffer_index   = 1;
    pip_desc.layout.attrs[ATTR_instance_main_aWorldScale].format       = SG_VERTEXFORMAT_FLOAT2;
    pip_desc.layout.attrs[ATTR_instance_main_aWorldScale].buffer_index = 1;
    pip_desc.layout.attrs[ATTR_instance_main_aUVScale].format          = SG_VERTEXFORMAT_FLOAT2;
    pip_desc.layout.attrs[ATTR_instance_main_aUVScale].buffer_index    = 1;

    set_alpha_blend(pip_desc);
    pip_desc.label = "pipeline";
    pipeline = sg_make_pipeline(pip_desc);
}

void InstancedSprite::Update(Math::Mat4 projection, Math::Mat4 view) {
    if (instances.empty()) return;
    sg_range range;
    range.ptr = instances.data();
    range.size = instances.size() * sizeof(InstanceData);
    sg_update_buffer(bindings.vertex_buffers[1], &range);
    dirty = false;
    
    vs_params.mvp = projection * view;
}

void InstancedSprite::Draw() {
    if (instances.empty()) return;
    sg_apply_pipeline(pipeline);
    sg_apply_bindings(&bindings);
    sg_apply_uniforms(UB_instance_params, SG_RANGE(vs_params));
    sg_draw(0, 6, static_cast<int>(instances.size()));
}

void InstancedSprite::Destroy() {
    instances.clear();
    sg_destroy_pipeline(pipeline);
}

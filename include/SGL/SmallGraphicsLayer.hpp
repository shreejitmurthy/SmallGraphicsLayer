#pragma once

#include <sokol_gfx.h>

#include "Math.hpp"

#include "genshaders/attributes.glsl.h"
#include "genshaders/ssprite.glsl.h"
#include "genshaders/instance.glsl.h"

#include <cstdint>
#include <vector>
#include <iostream>
#include <functional>
#include <unordered_map>

namespace SmallGraphicsLayer {

void EnableLogger();

typedef sg_color Colour;

struct Colours {
    static constexpr Colour Background = {0.15, 0.15, 0.2, 1};

    static constexpr Colour White  = {1, 1, 1, 1};
    static constexpr Colour Black  = {0, 0, 0, 1};
    static constexpr Colour Red    = {1, 0, 0, 1};
    static constexpr Colour Green  = {0, 1, 0, 1};
    static constexpr Colour Blue   = {0, 0, 1, 1};
    static constexpr Colour Yellow = {1, 1, 0, 1};

    static constexpr Colour Orange = {1, 0.5, 0, 1};
};

class Device {
public:
    // Consider a singleton accessor pattern in the future
    void Init(/* SGLFlags flags, */ int w = 0, int h = 0);
    void Clear(Colour clear_col = Colours::Background);
    void Refresh();
    void Shutdown();

    static float Width()  { return width;  }
    static float Height() { return height; }
    Math::Vec2 FrameSize() { return {static_cast<float>(width), static_cast<float>(height)}; };
private:
    static std::uint32_t width, height;
    sg_pass_action pass_action;
    sg_swapchain swapchain;
};

enum class RendererType {
    Attribute,  // basic attributes
    Single,     // sprites that render individually with one call
    Instanced   // more efficient instanced sprite rendering
};

class Renderer {
public:
    virtual ~Renderer() = default;
    virtual void Destroy() = 0;
    virtual RendererType Type() const = 0;
protected:
    sg_shader   shader   = {};
    sg_pipeline pipeline = {};
    sg_bindings bindings = {};
};

enum class UniformType {
    Invalid = 0,
    Float, Float2, Float3, Float4,
    Int, Int2, Int3, Int4,
    Mat4
};

// Custom shader program for AttributeBuilder
class AttributeProgram {
public:
    typedef struct {
        float resolution[2];
        float time;
    } fs_params;

    AttributeProgram(const std::string& frag);
    void ApplyDefaultUniforms(Math::Vec2 resolution = {0, 0}, float time = 0.f);
    fs_params GetUniformParams() { return params; }
    sg_shader_desc GetDesc() { return desc; }
    bool HasAppliedUniforms() { return applied_uniforms; }
private:
    sg_shader_desc desc = {};
    std::string frag_src_storage;
    int uniform_count = 0;
    std::unordered_map<std::string, int> uniforms;
    fs_params params;
    bool applied_uniforms;
};

enum class Primitives {
    Triangle = 3,
    Quad
};

typedef Math::Vec3 Position;
struct Index { std::uint16_t x, y, z; };

// Basic primitve and attribute renderer
class AttributeBuilder : public Renderer {
public:
    AttributeBuilder(Math::Vec2 framebufferSize, bool useNDC = false) : program("") {
        framebuf = framebufferSize;
        enable_ndc = useNDC;
    }
    AttributeBuilder(Math::Vec2 framebufferSize, AttributeProgram fragmentProgram, bool useNDC = false) : program(fragmentProgram) {
        framebuf = framebufferSize;
        enable_ndc = useNDC;
        use_custom_fragment = true;
    }
    AttributeBuilder& Begin(Primitives primative);
    AttributeBuilder& Vertex(Position pos, Colour col = Colours::Black, bool useNDC = false);
    AttributeBuilder& Index(Index index);
    
    void End();
    void Draw();
    void Draw(AttributeProgram program);
    void Destroy() override;
    
    RendererType Type() const override { return RendererType::Attribute; }
private:
    inline Position _Pixels2NDC(Position pos) {
        float x = ((2 * pos.x) / framebuf.x) - 1;
        float y = 1 - ((2 * pos.y) / framebuf.y);
        return Math::Vec3(x, y, pos.z);
    };

    int elements;
    int chunks;
    int expected_chunks;
    std::vector<float> vertices;
    std::vector<std::uint16_t> indices;

    bool enable_ndc;

    bool use_custom_fragment;
    AttributeProgram program;

    Math::Vec2 framebuf;

    sg_buffer vbuf = {};
    sg_buffer ibuf = {};
};

// Single sprite renderer, uses one draw call per sprite
class Sprite : public Renderer {
public:
    // TODO?
    Sprite(const std::string& path);
    Sprite(std::tuple<int, int, unsigned char*> data);
    void Draw(Math::Vec2 position, Math::Vec2 origin = {0, 0}, Math::Vec2 scale = {1, 1});
    void Destroy() override;

    RendererType Type() const override { return RendererType::Single; }
    
    float Width()  const { return size.x; }
    float Height() const { return size.y; }
private:
    sg_image image = {};
    sg_buffer vbuf = {};
    sg_buffer ibuf = {};
    sprite_params_t params;
    Math::Vec2 size;
};

struct InstanceData {
    Math::Vec2 offset;      // world-space X/Y
    Math::Vec2 uvOffset;    // which frame in the atlas
    Math::Vec2 worldScale;  // size of sprite in world eg. 64x32
    Math::Vec2 uvScale;     // size of sprite in uv space
};

inline Math::Vec2 get_tile_uv(Math::Vec2 tileIndex, Math::Vec2 tileSize, Math::Vec2 atlasSize) {
    return Math::Vec2(tileIndex.x * tileSize.x / float(atlasSize.x), tileIndex.y * tileSize.y / float(atlasSize.y));
}

// Instanced sprite renderer, uses one draw call for many sprites
class InstancedSpriteRenderer : public Renderer {
public:
    InstanceData createInstanceData(Math::Vec2 offset, Math::Vec2 tile_index, Math::Vec2 tile_size = {0, 0}) {
        if (tile_size == Math::Vec2(0, 0)) {
            tile_size = this->tile_size;
        }

        InstanceData ret;
        ret.offset = offset;
        ret.worldScale = tile_size;
        ret.uvOffset = get_tile_uv(tile_index, tile_size, Math::Vec2(w, h));
        ret.uvScale = Math::Vec2(
            tile_size.x / static_cast<float>(w),
            tile_size.y / static_cast<float>(h)
        );
        return ret;
    }

    void set_instance_data(const std::vector<InstanceData>& data) {
        instances = data;
        dirty = true;
    }

    void set_instance_data(std::vector<InstanceData>&& data) {
        instances = std::move(data);
        dirty = true;
    }

    InstancedSpriteRenderer(std::tuple<int, int, unsigned char*> data, Math::Vec2 tileSize);
private:
    unsigned int w, h, channels;
    instance_params_t vs_params;
    Math::Vec2 tile_size;
    std::vector<InstanceData> instances;
    bool dirty = false;
    bool initialised = false;
};
}

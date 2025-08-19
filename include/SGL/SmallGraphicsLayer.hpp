#pragma once

#include <sokol/sokol_gfx.h>

#include "Math.hpp"

#include "genshaders/attributes.glsl.h"

#include <cstdint>
#include <vector>
#include <iostream>
#include <functional>

namespace SmallGraphicsLayer {

typedef sg_color Colour;

struct Colours {
    static constexpr Colour White  = {1, 1, 1, 1};
    static constexpr Colour Red    = {1, 0, 0, 1};
    static constexpr Colour Green  = {0, 1, 0, 1};
    static constexpr Colour Blue   = {0, 0, 1, 1};
    static constexpr Colour Yellow = {1, 1, 0, 1};
};

class Device {
public:
    // Consider a singleton accessor pattern in the future
    void Init(/* SGLFlags flags, */ int w = 0, int h = 0);
    void Clear(Colour clear_col = {0.15f, 0.15f, 0.2f, 1.0f});
    void Refresh();
    void Shutdown();

    float Width()  { return width;  }
    float Height() { return height; }
    Math::Vec2 FrameSize() { return {static_cast<float>(width), static_cast<float>(height)}; };
private:
    std::uint32_t width, height;
    sg_pass_action pass_action;
    sg_swapchain swapchain;
};

typedef Math::Vec3 Position;
struct Index { std::uint16_t x, y, z; };


enum class RendererType {
    Attribute,  // basic attributes
    Single,     // sprites that render individually with one call
    Instanced   // more efficient instacned sprite rendering
};

class Renderer {
public:
    virtual ~Renderer() = default;
    virtual void Draw() = 0;
    virtual void Destroy() = 0;
    virtual RendererType type() const = 0;
protected:
    sg_shader   shader = {};
    sg_pipeline pipeline = {};
    sg_bindings bindings = {};
};

enum class Primitives {
    Triangle = 3,
    Quad
};

class AttributeBuilder : public Renderer {
public:
    AttributeBuilder(Math::Vec2 framebuffer_size, bool useNDC = false) { 
        framebuf = framebuffer_size;
        enable_ndc = useNDC;
    }
    AttributeBuilder& Begin(Primitives primative);
    AttributeBuilder& Vertex(Position pos, Colour col, bool useNDC = false);
    AttributeBuilder& Index(Index index);
    
    void End();
    void Draw() override;
    void Destroy() override;
    
    RendererType type() const override { return RendererType::Attribute; }
private:
    inline Position _Pixels2NDC(Position pos) {
        float x = ((2 * pos.x) / framebuf.x) - 1;
        float y = 1 - ((2 * pos.y) / framebuf.y);
        return { x, y, pos.z };
    };

    int elements;
    int chunks;
    int expected_chunks;
    std::vector<float> vertices;
    std::vector<std::uint16_t> indices;

    bool enable_ndc;

    Math::Vec2 framebuf;

    sg_buffer vbuf = {};
    sg_buffer ibuf = {};
};
};

// TODO: CPU-side sprite batching (like libGDX) and GPU instancing, gives more options.

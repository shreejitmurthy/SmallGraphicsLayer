#pragma once

#include <sokol/sokol_gfx.h>

#include "genshaders/attributes.glsl.h"

#include <cstdint>
#include <vector>
#include <iostream>

namespace SmallGraphicsLayer {

typedef sg_color Colour;

struct Colours {
    static constexpr Colour White = {1, 1, 1, 1};
    static constexpr Colour Red   = {1, 0, 0, 1};
    static constexpr Colour Green = {0, 1, 0, 1};
    static constexpr Colour Blue  = {0, 0, 1, 1};
};


class Device {
public:
    void Init(/* SGLFlags flags, */ int width = 0, int height = 0);
    void Clear(Colour clear_col = {0.15f, 0.15f, 0.2f, 1.0f});
    void Refresh();
    void Shutdown();
private:
    sg_pass_action pass_action;
    sg_swapchain swapchain;
};

struct Position { float x, y, z; };

enum class Primitives {
    Triangle = 3
};

class AttributeBuilder {
public:
    AttributeBuilder& Begin(Primitives primative);
    AttributeBuilder& Vertex(Position pos, Colour col);
    void End();
    void Draw();
    void Destroy();

private:
    int elements;
    std::vector<float> vertices;

    sg_shader shd = {};
    sg_buffer vbuf = {};
    sg_buffer ibuf = {};
    sg_bindings bindings = {};
    sg_pipeline pipeline = {};
};
};

#pragma once

#include <sokol/sokol_gfx.h>

#include <cstdint>

typedef sg_color SGLColour;

class SimpleGraphicsLayer {
public:
    void Init(/* SGLFlags flags, */ int width = 0, int height = 0);
    void Clear(SGLColour clear_col = {0.15f, 0.15f, 0.2f, 1.0f});
    void Render();
private:
    sg_pass_action pass_action;
    sg_swapchain swapchain;
};

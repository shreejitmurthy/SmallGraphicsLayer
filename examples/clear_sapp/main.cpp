#include "sokol_app.h"
#include "sokol_log.h"

#include "SGL/SmallGraphicsLayer.hpp"

SimpleGraphicsLayer sgl;

void init(void) {
    sgl.Init();
}

void frame(void) {
    sgl.Clear();
    sgl.Render();
}

void cleanup(void) {
    sg_shutdown();
}

int main(int argc, char** argv) {
    sapp_run((sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .width = 800,
        .height = 600,
        .window_title = "Window",
        .icon.sokol_default = true,
        .logger.func = slog_func,
    });
}

#include "sokol_app.h"
#include "sokol_log.h"

#include "SGL/SmallGraphicsLayer.hpp"

namespace sgl = SmallGraphicsLayer;

sgl::Device device;

void init(void) {
    device.init();
}

void frame(void) {
    device.clear();  // default clear colour
    device.render();
}

void cleanup(void) {
    device.shutdown();
}

int main(int argc, char** argv) {
    sapp_run((sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .width = 800,
        .height = 600,
        .window_title = "clear (sapp window)",
        .icon.sokol_default = true,
        .logger.func = slog_func,
    });
}

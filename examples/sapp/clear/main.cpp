#include "sokol_app.h"
#include "sokol_log.h"

#include "SGL/SmallGraphicsLayer.hpp"

namespace sgl = SmallGraphicsLayer;

sgl::Device device;

void init(void) {
    // Unlike the examples using SDL3, you won't need to provide the width and height of the screen
    device.Init();
}

void frame(void) {
    device.Clear();  // default clear colour
    device.Refresh();
}

void event(const sapp_event* e) {
    if (e->type == SAPP_EVENTTYPE_KEY_DOWN) {
        if (e->key_code == SAPP_KEYCODE_ESCAPE) {
            sapp_request_quit();
        }
    }
}

void cleanup(void) {
    device.Shutdown();
}

int main(int argc, char** argv) {
    sapp_run((sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .width = 800,
        .height = 600,
        .window_title = "clear (sapp window)",
        .icon.sokol_default = true,
        .logger.func = slog_func,
    });
}

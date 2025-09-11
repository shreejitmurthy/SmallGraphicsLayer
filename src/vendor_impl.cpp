// Required for SDL3 window backend (I'm pretty sure)

#define SOKOL_GFX_IMPL
#define SOKOL_GLCORE
#include "sokol/sokol_gfx.h"

#define SOKOL_LOG_IMPL
#include "sokol/sokol_log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

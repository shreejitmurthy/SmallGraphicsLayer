#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "SGL/SmallGraphicsLayer.hpp"
#include "SGL/AssetManager.hpp"

namespace sgl = SmallGraphicsLayer;
using namespace sgl::Math;

constexpr int screenWidth = 800;
constexpr int screenHeight = 600;

// Formula for isometric tiles.
Vec2 get_tile_offset(iVec2 pos, iVec2 tile_size, Vec2 global_offset = { screenWidth / 2, 0 }) {
    float x_pos = global_offset.x - tile_size.x / 2 + (pos.x - pos.y) * ((tile_size.x) / 2);
    float y_pos = global_offset.y + (pos.x + pos.y) * ((tile_size.y) / 2);
    return {x_pos, y_pos};
}

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return 1;
    }
    
    // Ensure an OpenGL context has been created.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow("instanced sprites (SDL3 window)", screenWidth, screenHeight, SDL_WINDOW_OPENGL);
    SDL_GLContext ctx = SDL_GL_CreateContext(window);
    bool open = true;

    SDL_GL_SetSwapInterval(1);

    sgl::EnableLogger();
    
    sgl::Device device;
    device.Init(screenWidth, screenHeight);

    std::string path = "examples/sdl3/instance/resources/tiles.png";

    const int mapWidth = 10;
    const int mapHeight = 10;

    const int tileWidth = 64;
    const int tileHeight = 32;

    sgl::AssetManager::Request(path, sgl::AssetType::Texture);
    auto texture = sgl::AssetManager::GetTexture(path);
    // Provide the atlas/tileset and the tile size.
    sgl::InstancedSpriteRenderer isr(texture->GetData(), {tileWidth, tileHeight});
    isr.Reserve(mapWidth * mapHeight);
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            isr.PushData(get_tile_offset({x, y}, {tileWidth, tileHeight}), {0, 0});
        }
    }

    SDL_Event event;

    while (open) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    open = false;
                    break;

                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE) {
                        open = false;
                    }
                    break;

                default:
                    break;
            }
        }

        device.Clear();  // default clear colour

        isr.Render();

        device.Refresh();

        SDL_GL_SwapWindow(window);
    }

    isr.Destroy();
    device.Shutdown();

    SDL_DestroyWindow(window);
    SDL_GL_DestroyContext(ctx);
    SDL_Quit();

    return 0;
}

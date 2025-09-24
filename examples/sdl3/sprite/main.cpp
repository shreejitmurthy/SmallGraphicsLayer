#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "SGL/SmallGraphicsLayer.hpp"
#include "SGL/AssetManager.hpp"

namespace sgl = SmallGraphicsLayer;

constexpr int screenWidth = 800;
constexpr int screenHeight = 600;

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return 1;
    }
    
    // Ensure an OpenGL context has been created.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow("sprite (SDL3 window)", screenWidth, screenHeight, SDL_WINDOW_OPENGL);
    SDL_GLContext ctx = SDL_GL_CreateContext(window);
    bool open = true;

    SDL_GL_SetSwapInterval(1);

    sgl::Device device;
    device.Init(screenWidth, screenHeight);

    std::string path = "examples/sdl3/sprite/freaker.png";

    // The asset manager handles async resource loading and freeing. Don't free the memory yourself.
    sgl::AssetManager::Request(path, sgl::AssetType::Texture);
    auto texture = sgl::AssetManager::GetTexture(path);
    sgl::Sprite sprite(texture->GetData());

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

        // Render the sprite in the centre of the screen
        // Sprites draw from the top-left of the image, but this
        // can be altered by the second origin parameter.
        sprite.Render({400, 300}, {sprite.Width() / 2, sprite.Height() / 2});

        // Note: Render() combines two functions of Sprite: Update() and Draw()
        // Update() takes the position, origin and scale. It does not use default parameters.
        // Draw() takes no parameters.
        // Render() has default parameters for origin and scale.

        device.Refresh();

        SDL_GL_SwapWindow(window);
    }

    sprite.Destroy();
    device.Shutdown();

    SDL_DestroyWindow(window);
    SDL_GL_DestroyContext(ctx);
    SDL_Quit();

    return 0;
}

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "SGL/SmallGraphicsLayer.hpp"

namespace sgl = SmallGraphicsLayer;

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return 1;
    }
    
    // Ensure an OpenGL context has been created.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    const int screenWidth = 800;
    const int screenHeight = 600;

    SDL_Window* window = SDL_CreateWindow("clear (SDL3 window)", screenWidth, screenHeight, SDL_WINDOW_OPENGL);
    SDL_GLContext ctx = SDL_GL_CreateContext(window);
    bool open = true;

    SDL_GL_SetSwapInterval(1);

    sgl::Device device;
    device.Init(screenWidth, screenHeight);

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
        device.Refresh();

        SDL_GL_SwapWindow(window);
    }

    device.Shutdown();

    SDL_DestroyWindow(window);
    SDL_GL_DestroyContext(ctx);
    SDL_Quit();

    return 0;
}

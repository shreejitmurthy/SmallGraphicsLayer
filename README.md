# SmallGraphicsLayer
SmallGraphicsLayer (SGL) is a graphics layer built on top of [sokol](https://github.com/floooh/sokol). It was made to support low level game development, whilst still giving control to the programmer.

SGL only uses sokol_gfx code, and supports OpenGL for macOS (Metal in the future), and D3D11 for Windows (untested). It does *not* provide a windowing backend, but you can (and should) specify what windowing backend you use. It's currently only been tested for SDL3 (`DSGL_BACKEND_SDL3`) and sokol_app (`DSGL_BACKEND_SAPP`), however defining SDL3 should allow for usage with glfw.

Examples are located in the `examples/` directory. They build with sokol and SDL3 using CMake. I recommend building with `ninja` for the fastest build times, but it's up to you. Remove the `-GNinja` if you don't want to use it:
```cmake
cmake -GNinja ..
cmake -S .. -B . -DSGL_BUILD_EXAMPLES=ON -DSGL_BACKEND_SAPP=OFF -DSGL_BACKEND_SDL3=ON
cmake --build .
./examples/sdl3/clear_sdl3/clear_sdl3
```

Currently, SGL can do the following:
- Clear screen/buffer
- Draw primitives
- Asynchronous assset loading and fetching
- Draw sprites normally (batched in the near future)

>[!IMPORTANT]
> If you don't wish to use the asset manager, you will need to use your own implementation. Read the source code to see what data each class (Sprite). `stb_image.h` is recommended, and is what is used for the asset manager.

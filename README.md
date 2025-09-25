# SmallGraphicsLayer
SmallGraphicsLayer (SGL) is a graphics layer built on top of [sokol](https://github.com/floooh/sokol). It was made to support low level game development, whilst still giving control to the programmer.

SGL only uses sokol_gfx code (with some functions from sokol_app and sokol_glue when using the sokol backend), and supports OpenGL for macOS (Metal in the future), and D3D11 for Windows (untested). It does *not* provide a windowing backend, but you can (and should) specify what windowing backend you use. It's currently only been tested for SDL3 (`DSGL_BACKEND_SDL3`) and sokol_app (`DSGL_BACKEND_SAPP`), however defining SDL3 should allow for usage with glfw.

## Why
This is an important question. Why did I make it? Well I wanted a library that allowed fine control over graphics related stuff (like `sokol` or [bgfx](https://github.com/bkaradzic/bgfx)) but also has high level functions that wrap over the repetitive stuff (like [raylib](https://github.com/raysan5/raylib)). I am modelling SGL entirely off what I'd like to use and my method of making games at the 'low level'.

## Examples
Examples are located in the `examples/` directory. They build with sokol and SDL3 using CMake. I recommend building with `ninja` for the fastest build times, but it's up to you. Remove the `-GNinja` if you don't want to use it:
```cmake
cmake -GNinja ..
cmake -S .. -B . -DSGL_BUILD_EXAMPLES=ON -DSGL_BACKEND_SAPP=OFF -DSGL_BACKEND_SDL3=ON
cmake --build .
./examples/sdl3/clear_sdl3/clear_sdl3
```
> [!NOTE]
> The `SmallGraphicsLayer::Math` namespace provides lightweight vector and matrix types (`Vec2`, `iVec2`, `Vec3`, `Vec4`, and `Mat4`) along with common operations. These are the same math utilities used internally by SGL, so they integrate seamlessly with the rest of the framework. However, if you can find a way to use your favourite other math library in conjunction with SGL, then feel free :). Below is a minimal example of using the Math functions from SGL:

```cpp
#include "SGL/Math.hpp"

namespace sgl = SmallGraphicsLayer;

int main() {
    using namespace sgl::Math;

    Mat4 transform = Mat4::translate({0.f, 1.f, 0.f}) * Mat4::rotateY(1.57f);

    Vec3 p{1.f, 0.f, 0.f};
    Vec3 result = transform.multiplyPoint(p);
}
```
## Capabilities
Currently, SGL can do the following:
- Math functions similar to [glm](https://github.com/g-truc/glm)
- Clear screen/buffer
- Primitive drawing (w/ custom fragment shaders as well)
- Asynchronous assset loading and fetching (it kinda sucks right now, but it works)
- Sprite drawing (instancing and batching (*todo*))


>[!IMPORTANT]
> The Utils provided by SGL can load a file into a string and attempt to resolve the full path of a relative path. Resource loading itself is managed by the Asset Manager, though you’re not required to use it. Instead, check what form of data each resource-using class expects, and pass that data in directly using any other method you wish.
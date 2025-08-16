# cmake_minimum_required(VERSION 3.29)
# project(SmallGraphicsLayer VERSION 0.1.0 LANGUAGES CXX OBJCXX)

# set(CMAKE_CXX_STANDARD 17)

# option(SGL_BUILD_EXAMPLES "Build example apps" ON)
# option(SGL_BACKEND_SAPP "Build examples using sokol_app" ON)
# option(SGL_BACKEND_SDL3 "Build examples using SDL3" OFF)

# set(SGL_SOURCES "")

# if (SGL_BACKEND_SDL3)
#     set(SGL_SOURCES src/SmallGraphicsLayer.cpp)
# elseif(SGL_BACKEND_SAPP)
#     set(SGL_SOURCES src/sokol_impl.mm src/SmallGraphicsLayer.cpp)
# else()
#     message(FATAL_ERROR "Define one backend: -DSGL_BACKEND_SDL3=ON or -DSGL_BACKEND_SAPP=ON")
# endif()

# add_library(SmallGraphicsLayer ${SGL_SOURCES})

# # Ensure only one backend is chosen
# if (SGL_BACKEND_SDL3 AND SGL_BACKEND_SAPP)
#     message(FATAL_ERROR "Choose only one backend: SDL3 OR sokol_app")
# elseif (SGL_BACKEND_SDL3)
#     target_compile_definitions(SmallGraphicsLayer PUBLIC WINDOW_SDL=1)
# elseif (SGL_BACKEND_SAPP)
#     target_compile_definitions(SmallGraphicsLayer PUBLIC WINDOW_SAPP=1)
# else()
#     message(FATAL_ERROR "Define one backend: -DSGL_BACKEND_SDL3=ON or -DSGL_BACKEND_SAPP=ON")
# endif()

# target_include_directories(SmallGraphicsLayer PUBLIC
#     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#     $<INSTALL_INTERFACE:include>
# )

# target_compile_features(SmallGraphicsLayer PUBLIC cxx_std_20)

# target_include_directories(SmallGraphicsLayer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

# # check again to see if this sucks
# if(APPLE)
#     target_link_libraries(SmallGraphicsLayer PUBLIC 
#         "-framework OpenGL"
#         "-framework Cocoa"
#         "-framework QuartzCore"
#     )
# endif()

# if (SGL_BUILD_EXAMPLES)
#     add_subdirectory(examples)
# endif()

cmake_minimum_required(VERSION 3.29)
project(SmallGraphicsLayer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

option(SGL_BUILD_EXAMPLES "Build example apps" ON)
option(SGL_BACKEND_SAPP "Build using sokol_app" OFF)
option(SGL_BACKEND_SDL3 "Build using SDL3" ON)

# pick sources based on backend
set(SGL_SOURCES src/SmallGraphicsLayer.cpp)
if (SGL_BACKEND_SAPP)
    enable_language(OBJCXX)
    list(APPEND SGL_SOURCES src/sokol_impl.mm)
elseif (SGL_BACKEND_SDL3)
    list(APPEND SGL_SOURCES src/sokol_gfx_impl.cpp)
endif()

add_library(SmallGraphicsLayer ${SGL_SOURCES})

# exactly one backend
if (SGL_BACKEND_SDL3 AND SGL_BACKEND_SAPP)
    message(FATAL_ERROR "Choose only one backend")  
elseif (SGL_BACKEND_SDL3)
    target_compile_definitions(SmallGraphicsLayer PUBLIC WINDOW_SDL=1)  
elseif (SGL_BACKEND_SAPP)
    target_compile_definitions(SmallGraphicsLayer PUBLIC WINDOW_SAPP=1) 
else()
    message(FATAL_ERROR "Define one backend")   
endif()

target_include_directories(SmallGraphicsLayer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)

# In the future use Metal?
if (APPLE)
    target_link_libraries(SmallGraphicsLayer PUBLIC 
        "-framework Cocoa" 
        "-framework QuartzCore" 
        "-framework OpenGL"
        # "-framework Metal"
        # "-framework MetalKit"
        "-stdlib=libc++"
    )
endif()

if (SGL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()


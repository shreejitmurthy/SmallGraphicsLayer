cmake_minimum_required(VERSION 3.29)
project(SmallGraphicsLayer VERSION 0.8.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory("vendor/spdlog")

option(SGL_BUILD_EXAMPLES "Build example apps" ON)
option(SGL_BACKEND_SAPP "Build using sokol_app" OFF)
option(SGL_BACKEND_SDL3 "Build using SDL3" ON)

# pick sources based on window backend
set(SGL_SOURCES "src/SmallGraphicsLayer.cpp" "src/AssetManager.cpp" "src/Log.cpp")
if (SGL_BACKEND_SAPP)
    enable_language(OBJCXX)
    list(APPEND SGL_SOURCES src/vendor_impl.mm)
elseif (SGL_BACKEND_SDL3)
    list(APPEND SGL_SOURCES src/vendor_impl.cpp)
endif()

add_library(SmallGraphicsLayer ${SGL_SOURCES})

# exactly one window backend
if (SGL_BACKEND_SDL3 AND SGL_BACKEND_SAPP)
    message(FATAL_ERROR "Choose only one window backend")  
elseif (SGL_BACKEND_SDL3)
    target_compile_definitions(SmallGraphicsLayer PUBLIC WINDOW_SDL=1)  
elseif (SGL_BACKEND_SAPP)
    target_compile_definitions(SmallGraphicsLayer PUBLIC WINDOW_SAPP=1) 
else()
    message(FATAL_ERROR "Define one window backend")   
endif()

target_include_directories(SmallGraphicsLayer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)

# In the future use Metal?
if (APPLE)  # todo: build for other platforms
    target_link_libraries(SmallGraphicsLayer PUBLIC 
        "-framework Cocoa" 
        "-framework QuartzCore" 
        "-framework OpenGL"
        # "-framework Metal"
        # "-framework MetalKit"
        "-stdlib=libc++"
        spdlog::spdlog
    )
endif()

if (SGL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()


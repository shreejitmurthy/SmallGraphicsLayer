cmake_minimum_required(VERSION 3.29)
project(SmallGraphicsLayer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

option(SGL_BUILD_EXAMPLES "Build example apps" ON)
option(SGL_BACKEND_SOKOL_APP "Build examples using sokol_app" OFF)
option(SGL_BACKEND_SDL3 "Build examples using SDL3" ON)

set(SGL_SOURCES
    src/SmallGraphicsLayer.cpp
)

add_library(SmallGraphicsLayer ${SGL_SOURCES})

target_include_directories(SmallGraphicsLayer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(SmallGraphicsLayer PUBLIC cxx_std_20)

target_include_directories(SmallGraphicsLayer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

if (SGL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

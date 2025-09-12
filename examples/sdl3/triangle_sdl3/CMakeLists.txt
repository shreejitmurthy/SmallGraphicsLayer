cmake_minimum_required(VERSION 3.29)

add_executable(triangle_sdl3
    main.cpp
)

target_link_libraries(triangle_sdl3 PRIVATE
    SmallGraphicsLayer
    SDL3::SDL3
)

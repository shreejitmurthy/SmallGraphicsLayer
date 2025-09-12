cmake_minimum_required(VERSION 3.29)

add_executable(triangle
    main.cpp
)

target_link_libraries(triangle PRIVATE
    SmallGraphicsLayer
    SDL3::SDL3
)

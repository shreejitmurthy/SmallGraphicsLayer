cmake_minimum_required(VERSION 3.29)

add_executable(program
    main.cpp
)

target_link_libraries(program PRIVATE
    SmallGraphicsLayer
    SDL3::SDL3
)

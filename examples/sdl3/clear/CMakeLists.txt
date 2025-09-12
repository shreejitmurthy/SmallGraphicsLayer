cmake_minimum_required(VERSION 3.29)

add_executable(clear
    main.cpp
)

target_link_libraries(clear PRIVATE
    SmallGraphicsLayer
    SDL3::SDL3
)
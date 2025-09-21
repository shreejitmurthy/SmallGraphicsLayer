cmake_minimum_required(VERSION 3.29)

add_executable(fragment
    main.cpp
)

target_link_libraries(fragment PRIVATE
    SmallGraphicsLayer
    SDL3::SDL3
)

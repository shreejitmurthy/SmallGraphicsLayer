cmake_minimum_required(VERSION 3.29)

add_executable(clear_sdl3
    main.cpp
)

target_link_libraries(clear_sdl3 PRIVATE
    SmallGraphicsLayer
    SDL3::SDL3
)

# if example loads assets relative to the repo, set a working dir
# set_target_properties(hello_sdl PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

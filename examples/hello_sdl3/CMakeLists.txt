cmake_minimum_required(VERSION 3.29)

add_executable(hello_sdl
    main.cpp
)

target_link_libraries(hello_sdl PRIVATE
    SmallGraphicsLayer
    SDL3::SDL3
)

# (Optional) if your example code needs to #ifdef on the backend
# target_compile_definitions(hello_sdl PRIVATE SGL_USE_SDL3=1)

# if (MSVC)
#     target_compile_options(hello_sdl PRIVATE /W4 /permissive-)
# else()
#     target_compile_options(hello_sdl PRIVATE -Wall -Wextra -Wpedantic)
# endif()

# If your example loads assets relative to the repo, set a working dir (optional)
# set_target_properties(hello_sdl PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

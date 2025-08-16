cmake_minimum_required(VERSION 3.29)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/sokol)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL EXCLUDE_FROM_ALL)

# TODO: Way to configure this
add_subdirectory(clear_sdl3)
# add_subdirectory(clear_sapp)

cmake_minimum_required(VERSION 3.29)

if (NOT SGL_BACKEND_SDL3)
  message(FATAL_ERROR "Examples are configured for SDL3 only right now. Turn SGL_BACKEND_SDL3=ON.")
endif()

# Bring in the SDL3 submodule (kept out of 'all' so it only builds when linked)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL EXCLUDE_FROM_ALL)

# Add your examples
add_subdirectory(clear_sdl3)

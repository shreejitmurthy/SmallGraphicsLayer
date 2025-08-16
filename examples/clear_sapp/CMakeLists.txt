# examples/clear_sapp/CMakeLists.txt

project(clear_sapp LANGUAGES CXX OBJCXX)

add_executable(clear_sapp
    main.cpp
    ../sokol.mm
)

if(APPLE)
    target_link_libraries(clear_sapp PRIVATE 
        SmallGraphicsLayer
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework QuartzCore"
    )
else()
    target_link_libraries(clear_sapp PRIVATE SmallGraphicsLayer)
endif()


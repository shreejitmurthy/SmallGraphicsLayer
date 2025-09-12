# examples/clear/CMakeLists.txt

project(clear LANGUAGES CXX OBJCXX)

add_executable(clear
    main.cpp
    ../sokol.mm
)

if(APPLE)
    target_link_libraries(clear PRIVATE 
        SmallGraphicsLayer
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework QuartzCore"
    )
else()
    target_link_libraries(clear PRIVATE SmallGraphicsLayer)
endif()


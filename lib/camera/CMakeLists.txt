add_library(camera STATIC
        include/Camera.hpp
        src/Camera.cpp
)

target_link_libraries(camera
    PRIVATE
        math
)

target_include_directories(camera
        PUBLIC
        ../camera/include
        ../math
)
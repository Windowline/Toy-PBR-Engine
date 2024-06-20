########## engine ##########
add_library(engine STATIC
        include/Engine.hpp
        src/Engine.cpp

        include/Scene.hpp
        src/Scene.cpp

        include/Node.hpp
        src/Node.cpp
)

target_include_directories(engine
PUBLIC
        ../engine/include
        ../shader/include
        ../basic_mesh/include
        ../camera/include
        ../frame_buffer/include
        ../camera/include
        ../math
        ../util
        ../model/include
)

target_link_libraries(engine
 PUBLIC
# PRIVATE
        shader
        mesh
        camera
        frame_buffer
        math
        util
        model
)
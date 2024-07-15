########## engine ##########
add_library(engine STATIC
        include/Engine.hpp
        src/Engine.cpp

        include/PBRScene.hpp
        src/PBRScene.cpp

        include/RayTraceScene.hpp
        src/RayTraceScene.cpp

        include/Node.hpp
        src/Node.cpp

        include/BVH.hpp
        include/Scene.hpp
)

configure_file(${CMAKE_SOURCE_DIR}/config/PathInfo.h.in ${CMAKE_BINARY_DIR}/config/PathInfo.h)

target_include_directories(engine
PUBLIC
        ../engine/include
        ../shader/include
        ../mesh/include
        ../camera/include
        ../frame_buffer/include
        ../camera/include
        ../ibl/include
        ../stb_image_loader/include
        ../math
        ../util
        ${CMAKE_BINARY_DIR}/config
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
        ibl
        stb_image_loader
)
########## engine ##########
add_library(engine STATIC
        include/Engine.hpp
        src/Engine.cpp

        include/PBRScene.hpp
        src/PBRScene.cpp

        include/RayTraceScene.hpp
        src/RayTraceScene.cpp

        include/ModelNode.hpp
        src/ModelNode.cpp

        include/BVH.hpp
        include/Scene.hpp
        include/UXUtil.hpp

        include/Camera.hpp
        src/Camera.cpp

        include/FrameBufferObject.hpp
        src/FrameBufferObject.cpp

        include/IBLPreprocessor.hpp
        src/IBLPreprocessor.cpp
)

configure_file(${CMAKE_SOURCE_DIR}/config/PathInfo.h.in ${CMAKE_BINARY_DIR}/config/PathInfo.h)

target_include_directories(engine
PUBLIC
        ../engine/include
        ../shader/include
        ../mesh/include
        ../frame_buffer/include
        ../stb_image_loader/include
        ../math
        ../util
        ${CMAKE_BINARY_DIR}/config
)

target_link_libraries(engine
 PUBLIC
        shader
        mesh
        math
        util
        stb_image_loader
)
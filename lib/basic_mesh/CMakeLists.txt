add_library(mesh STATIC
        include/Triangle.hpp
        src/Triangle.cpp

        include/Cube.hpp
        src/Cube.cpp

        include/FullQuad.hpp
        src/FullQuad.cpp

        ../shader/src/SSAOBlurShader.cpp
        include/Sphere.hpp

        src/Sphere.cpp
        include/BasicMeshInterface.h

        include/Room.hpp
        src/Room.cpp
)

target_include_directories(mesh
        PUBLIC
        ../basic_mesh/include
        ../math
        ../util
        ../shader/include
)

target_link_libraries(mesh
PUBLIC
       math
       shader
       util
)
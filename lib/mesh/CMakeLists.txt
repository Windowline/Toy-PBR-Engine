add_library(mesh STATIC

        include/Triangle.hpp
        src/Triangle.cpp
        include/Cube2.hpp
        src/Cube2.cpp
        include/FullQuad.hpp
        src/FullQuad.cpp
        ../shader/src/SSAOBlurShaderTmp.cpp
        include/Sphere2.hpp
        src/Sphere2.cpp
        include/BasicMeshInterface.h
        include/Room2.hpp
        src/Room2.cpp
)

target_include_directories(mesh
        PUBLIC
        ../mesh/include
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
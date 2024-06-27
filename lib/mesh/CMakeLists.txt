add_library(mesh STATIC

        include/Cube.hpp
        src/Cube.cpp

        include/FullQuad.hpp
        src/FullQuad.cpp

        ../shader/src/SSAOBlurShader.cpp
        include/Sphere.hpp

        src/Sphere.cpp
        include/MeshBasic.h

        include/Room.hpp
        src/Room.cpp

        include/Model.hpp
        src/Model.cpp
        include/Plane.hpp
        src/Plane.cpp
)

target_include_directories(mesh
        PUBLIC
        include
        ../math
        ../util
        ../shader/include
)

find_package(assimp CONFIG REQUIRED)

target_link_libraries(mesh
PUBLIC
       math
       shader
       util
PRIVATE
       assimp::assimp
)
add_library(mesh STATIC
        include/Cube.hpp
        src/Cube.cpp

        include/LongCube.hpp
        src/LongCube.cpp

        include/Mesh.hpp
        src/Mesh.cpp

        include/Plane.hpp
        src/Plane.cpp

        include/Room.hpp
        src/Room.cpp

        include/Sphere.hpp
        src/Sphere.cpp

        include/Triangle.hpp
        src/Triangle.cpp
        include/Cube2.hpp
        src/Cube2.cpp
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
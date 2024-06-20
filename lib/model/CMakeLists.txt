add_library(model STATIC
    include/Model.hpp
    src/Model.cpp
)

find_package(assimp CONFIG REQUIRED)

target_include_directories(model
PUBLIC
#    ../include
    ../math
    ../basic_mesh/include
)

target_link_libraries(model
PUBLIC
   math
PRIVATE
   assimp::assimp
   mesh
)
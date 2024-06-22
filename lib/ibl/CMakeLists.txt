add_library(ibl STATIC
    include/IBLPreprocessor.hpp
    src/IBLPreprocessor.cpp
)

target_include_directories(ibl
PUBLIC
    include
    ../shader/include
    ../camera/include
    ../mesh/include
)

target_link_libraries(ibl
PRIVATE
   shader
   camera
   mesh
)
# TODO
# -DCMAKE_TOOLCHAIN_FILE="/Users/bagchangseon/vcpkg/scripts/buildsystems/vcpkg.cmake"
# ./vcpkg install glad
# ./vcpkg install glfw
# ./vcpkg install ASSIMP
# ./vcpkg install stb

cmake_minimum_required(VERSION 3.28)
project(ToyRenderer)

set(CMAKE_CXX_STANDARD 17)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_subdirectory(lib)

#add_executable(app main.cpp
#        lib/util/GLUtilGeometry.hpp
#        lib/mesh/include/Model.hpp
#        lib/mesh/src/Model.cpp
#        lib/ibl/include/IBLPreprocessor.hpp
#        lib/ibl/src/IBLPreprocessor.cpp
#        lib/stb_image_loader/include/ImageLoader.hpp
#        lib/stb_image_loader/src/ImageLoader.cpp)

add_executable(app main.cpp)


target_link_libraries(app
        PRIVATE
        engine
        glad::glad
        glfw
)



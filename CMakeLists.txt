# TODO
# -DCMAKE_TOOLCHAIN_FILE="/Users/bagchangseon/vcpkg/scripts/buildsystems/vcpkg.cmake"
# ./vcpkg install glad
# ./vcpkg install glfw
# ./vcpkg install ASSIMP
# ./vcpkg install stb

cmake_minimum_required(VERSION 3.2)
project(ToyRenderer)

set(CMAKE_CXX_STANDARD 17)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_subdirectory(lib)

add_executable(app_pbr main_pbr.cpp)

target_link_libraries(app_pbr
        PRIVATE
        engine
        glad::glad
        glfw
)

add_executable(app_ray_trace main_ray_trace.cpp)

target_link_libraries(app_ray_trace
        PRIVATE
        engine
        glad::glad
        glfw
)



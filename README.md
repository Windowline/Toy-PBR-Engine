# Small 3D Engine(Backend: OpenGL)

##


## Build Tools
- CMake 3.2
- vcpkg

## Install Dependencies
```
./vcpkg install glad
./vcpkg install glfw
./vcpkg install ASSIMP
./vcpkg install stb
```

## Build && Run
```
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="{your_vcpkg_path}/scripts/buildsystems/vcpkg.cmake"
make
./app
```



  

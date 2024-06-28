# Toy Real Time PBR Engine
- Cross Platform Desktop Sample 
- Backend API: OpenGL
- Rendering Effects: Shadow, SSAO(Screen-Space Ambient Occlusion), IBL, Deferred Rendering

- Control UX
  - Mouse: Rotate view-space basis 
  - Keybord
    - W: Move to Z of view-space
    - S: Move to -Z of view-space
    - A: Move to -X of view-space
    - D: Move to X of view-space



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

## Build & Run
```
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="{your_vcpkg_path}/scripts/buildsystems/vcpkg.cmake"
make
./app
```



  

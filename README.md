# Toy Real Time PBR Engine
- Cross Platform Desktop Sample (Backend: OpenGL)
  
- Rendering Depandancy Graph
  ![image](https://github.com/Windowline/ToyRenderer/assets/17508384/0c099a47-a9aa-44df-b520-35fddf136fc0)

- [Metalic: 0.9 / Roughness: 0.1]
  ![image](https://github.com/Windowline/ToyRenderer/assets/17508384/28df3a4e-9c4d-4cca-bb7d-85bf422d8151)

- [Metalic: 0.6 / Roughness: 0.4]
  ![image](https://github.com/Windowline/ToyRenderer/assets/17508384/9166d4bf-ed49-4649-96b0-6dce5239d380)


- Camera Control UX
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



  

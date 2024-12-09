# Toy Renderer(practice rendering using OpenGL 3.3)
- Desktop App
- Language: C++17
- Graphics API: OpenGL 3.3
- Limitations
  - There are some shortcomings in the architecture (e.g., dynamic Frame Graph, Resource Alias, etc.).
  - The reason is that I decided there would be more to gain by implementing these features later with modern APIs such as Vulkan, Metal, and DX12.
  - So I focused on rendering effects and did not allocate additional time to the architecture before concluding the project.
- Implementing Effects
  - 1 Rasterization Rendering (IBL, Deferred Lighting, Ambient Occlusion, ETC)
    - Render Dependency Graph
      <img width="1505" alt="Render Graph" src="https://github.com/user-attachments/assets/afa3c0ea-f917-4f72-960d-a927a8381afa">


    - [Metalic: 0.9 / Roughness: 0.1]
      ![image](https://github.com/Windowline/ToyRenderer/assets/17508384/28df3a4e-9c4d-4cca-bb7d-85bf422d8151)

    - [Metalic: 0.6 / Roughness: 0.4]
      ![image](https://github.com/Windowline/ToyRenderer/assets/17508384/9166d4bf-ed49-4649-96b0-6dce5239d380)
      
  - 2 Ray Tracing Rendering (Extensions related to trace and acceleration are not being used.)
      ![image](https://github.com/user-attachments/assets/269ab8cb-f933-4c4b-9ace-7984c17ab1b3)

      BVH - It is uploaded to GPU via TexelBufferObject (SSBO is currently unavailable.)
      ![image](https://github.com/user-attachments/assets/45fe607a-e085-43a8-a6ff-0ad41e248564)

  
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

## Build
```
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="{your_vcpkg_path}/scripts/buildsystems/vcpkg.cmake"
make
```

## Run Sample App
Rasterization Rendering
```
./app_pbr
```

Ray Tracing Rendering
```
./app_ray_trace
```

## Reference
Thanks to https://learnopengl.com/


  

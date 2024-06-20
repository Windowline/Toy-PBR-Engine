add_library(shader STATIC

        include/ShaderManager.hpp
        src/ShaderManager.cpp

        include/BasicShader.hpp
        src/BasicShader.cpp

        include/ShadowDepthShader.hpp
        src/ShadowDepthShader.cpp
        include/GBufferShader.hpp
        src/GBufferShader.cpp
        include/TexturePassShader.hpp
        src/TexturePassShader.cpp
        include/DepthDebugShader.hpp
        src/DepthDebugShader.cpp
        include/SSAOShader.hpp
        src/SSAOShader.cpp
        include/SSAOBlurShader.hpp
        include/DeferredLightingShader.hpp
        src/DeferredLightingShader.cpp
        include/AlbedoColorShader.hpp
        src/AlbedoColorShader.cpp
)

target_link_libraries(shader
        PUBLIC
        glad::glad
        glfw
        util
)

target_include_directories(shader
        PUBLIC
        include
        ../math
        ../util
)
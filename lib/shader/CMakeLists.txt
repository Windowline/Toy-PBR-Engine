add_library(shader STATIC
        include/ShaderBase.hpp

        include/AlbedoColorShader.hpp
        src/AlbedoColorShader.cpp

        include/BasicLightingShader.hpp
        src/BasicLightingShader.cpp

        include/DeferredLightingShader.hpp
        src/DeferredLightingShader.cpp

        include/GBufferShader.hpp
        src/GBufferShader.cpp

        include/ShadowDepthShader.hpp
        src/ShadowDepthShader.cpp

        include/SSAOBlurShader.hpp
        src/SSAOBlurShader.cpp

        include/SSAOShader.hpp
        src/SSAOShader.cpp

        include/TexturePassShader.hpp
        src/TexturePassShader.cpp

        include/ShaderManager.hpp
        src/ShaderManager.cpp

        include/BasicShader.hpp
        src/BasicShader.cpp

        include/ShadowDepthShaderTmp.hpp
        src/ShadowDepthShaderTmp.cpp
        include/GBufferShaderTmp.hpp
        src/GBufferShaderTmp.cpp
        include/TexturePassShaderTmp.hpp
        src/TexturePassShaderTmp.cpp
        include/DepthDebugShaderTmp.hpp
        src/DepthDebugShaderTmp.cpp
        include/SSAOShaderTmp.hpp
        src/SSAOShaderTmp.cpp
        include/SSAOBlurShaderTmp.hpp
        include/DeferredLightingShaderTmp.hpp
        src/DeferredLightingShaderTmp.cpp
        include/AlbedoColorShaderTmp.hpp
        src/AlbedoColorShaderTmp.cpp
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
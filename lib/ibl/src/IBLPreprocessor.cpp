#include "IBLPreprocessor.hpp"
#include "ShaderManager.hpp"
#include "IBLPreprocessDiffuseShaders.hpp"
#include "IBLPreprocessSpecularShaders.hpp"
#include "FullQuad.hpp"
#include "Cube.hpp"
#include "ImageLoader.hpp"
#include <glad/glad.h>
#include <array>

IBLPreprocessor::IBLPreprocessor(shared_ptr<ShaderManager> shaderManager, string_view path) : _shaderManager(shaderManager), _path(path) {
    _fullCube = make_unique<Cube>(2, vec3(1, 1, 1));
    _fullQuad = make_unique<FullQuad>();
}

void IBLPreprocessor::build() {
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    // set depth function to less than AND equal for skybox depth trick.
    glDepthFunc(GL_LEQUAL);
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    array<mat4, 6> captureViews = {
            Camera::createViewMatrix(vec3(1.0f,  0.0f,  0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),
            Camera::createViewMatrix(vec3(-1.0f, 0.0f,  0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),
            Camera::createViewMatrix(vec3(0.0f,  1.0f,  0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f,  1.0f)),
            Camera::createViewMatrix(vec3(0.0f,  -1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f,  -1.0f)),
            Camera::createViewMatrix(vec3(0.0f,  0.0f,  1.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),
            Camera::createViewMatrix(vec3(0.0f,  0.0f,  -1.0f),vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f))
    };

    mat4 captureProj = mat4::Frustum(100.0, 100.0, 90.0, 0.01, 10.0);

    glGenFramebuffers(1, &_captureFBO);
    glGenRenderbuffers(1, &_captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, _captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _captureRBO);

    renderEnvironmentCubeMapFromHDR(_captureFBO, captureViews, captureProj);
    renderIrradianceCubeMap(_captureFBO, _captureRBO, captureViews, captureProj);
    renderPrefilterCubemap(_captureFBO, _captureRBO, captureViews, captureProj);
    renderBRDFLUT(_captureFBO, _captureRBO);
}

void IBLPreprocessor::renderEnvironmentCubeMapFromHDR(unsigned int captureFBO, const array<mat4, 6>& captureViews, const mat4& captureProj) {
    // pbr: load the HDR environment map
    // ---------------------------------
    int width, height, nrComponents;
    float* data = Stb::loadImageFloat(_path.data(), &width, &height, &nrComponents, 0, true);
    assert(data != NULL);
    glGenTextures(1, &_hdrTexture);
    glBindTexture(GL_TEXTURE_2D, _hdrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT,data); // note how we specify the texture's data value to be float
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    Stb::free(data);


    // pbr: setup cubemap to render to and attach to framebuffer
    // ---------------------------------------------------------
    glGenTextures(1, &_envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _envCubemap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    auto equi2cubeShader = _shaderManager->setActiveShader<EquirectangularToCubemapShader>(eShaderProgram_EquirectangularToCubemap);
    equi2cubeShader->projMatUniformMatrix4fv(captureProj.pointer());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _hdrTexture);
    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i) {
        equi2cubeShader->viewMatUniformMatrix4fv(captureViews[i].pointer());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _fullCube->render();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, _envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void IBLPreprocessor::renderIrradianceCubeMap(unsigned int captureFBO, unsigned int captureRBO, const array<mat4, 6>& captureViews, const mat4& captureProj) {
    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    // --------------------------------------------------------------------------------
    glGenTextures(1, &_irradianceCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _irradianceCubeMap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
    // -----------------------------------------------------------------------------
    auto irradianceShader = _shaderManager->setActiveShader<IrradianceShader>(eShaderProgram_Irradiance);
    irradianceShader->projMatUniformMatrix4fv(captureProj.pointer());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _envCubemap);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i) {
        irradianceShader->viewMatUniformMatrix4fv(captureViews[i].pointer());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _irradianceCubeMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _fullCube->render();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void IBLPreprocessor::renderPrefilterCubemap(unsigned int captureFBO, unsigned int captureRBO, const array<mat4, 6>& captureViews, const mat4& captureProj) {
    // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
    // --------------------------------------------------------------------------------
    glGenTextures(1, &_prefilterCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _prefilterCubeMap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


    auto prefilterShader = _shaderManager->setActiveShader<PrefilterShader>(eShaderProgram_Prefilter);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _envCubemap);
    prefilterShader->projMatUniformMatrix4fv(captureProj.pointer());

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth  = static_cast<unsigned int>(128 * pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader->roughnesUniformLocation1f(roughness);

        for (unsigned int i = 0; i < 6; ++i) {
            prefilterShader->viewMatUniformMatrix4fv(captureViews[i].pointer());
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _prefilterCubeMap, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _fullCube->render();
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void IBLPreprocessor::renderBRDFLUT(unsigned int captureFBO, unsigned int captureRBO) {
    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    glGenTextures(1, &_brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, _brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    auto brdfShader = _shaderManager->setActiveShader<BRDFShader>(eShaderProgram_BRDF);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _fullQuad->render();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}




IBLPreprocessor::~IBLPreprocessor() {
    if (_hdrTexture != 0)
        glDeleteTextures(1, &_hdrTexture);

    if (_irradianceCubeMap != 0)
        glDeleteTextures(1, &_irradianceCubeMap);

    if (_prefilterCubeMap != 0)
        glDeleteTextures(1, &_prefilterCubeMap);

    if (_envCubemap != 0)
        glDeleteTextures(1, &_envCubemap);

    if (_brdfLUTTexture != 0)
        glDeleteTextures(1, &_brdfLUTTexture);

    if (_captureFBO != 0)
        glDeleteFramebuffers(1, &_captureFBO);

    if (_captureRBO != 0)
        glDeleteRenderbuffers(1, &_captureRBO);
}





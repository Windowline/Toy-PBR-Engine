
#include "ShaderManager.hpp"
#include "BasicShader.hpp"
#include "GBufferShader.hpp"
#include "TexturePassShader.hpp"
#include "ShadowDepthShader.hpp"
#include "SSAOShader.hpp"
#include "SSAOBlurShader.hpp"
#include "IBLPreprocessDiffuseShaders.hpp"
#include "IBLPreprocessSpecularShaders.hpp"
#include "BGShader.hpp"
#include "PBRShader.hpp"
#include "DeferredPBRShader.hpp"
#include "BVHRayTraceShader.hpp"

using namespace std;

ShaderManager::ShaderManager()
    : _activeShader(nullptr) {
}

void ShaderManager::addShader(int shaderId, shared_ptr<BasicShader> const & shaderProgram ) {
    auto it = _shaderMap.find(shaderId);

    if(it == _shaderMap.end()) {
        _shaderMap.insert({shaderId, shaderProgram});
    }
}

void ShaderManager::removeShader(int shaderId ) {
    _shaderMap.erase(shaderId);
}

shared_ptr<BasicShader> ShaderManager::findShader(int shaderId) const {
    auto it = _shaderMap.find(shaderId);

    if (it != _shaderMap.end()) {
        if (!it->second->isLoaded()) {
            it->second->load();
        }
        return it->second;
    }

    return nullptr;
}


ShaderManager::~ShaderManager() {
    unloadAllPrograms();
    _shaderMap.clear();
}

void ShaderManager::loadAllPrograms() {
    if (_shaderMap.empty()) {
        _shaderMap = {
            {eShaderProgram_Basic,                    make_shared<BasicShader>()},
            {eShaderProgram_GBuffer,                  make_shared<GBufferShader>()},
            {eShaderProgram_TexturePass,              make_shared<TexturePassShader>() },
            {eShaderProgram_ShadowDepth,              make_shared<ShadowDepthShader>()},
            {eShaderProgram_SSAO,                     make_shared<SSAOShader>()},
            {eShaderProgram_SSAO_BLUR,                make_shared<SSAOBlurShader>()},
            {eShaderProgram_EquirectangularToCubemap, make_shared<EquirectangularToCubemapShader>()},
            {eShaderProgram_Irradiance,               make_shared<IrradianceShader>()},
            {eShaderProgram_Prefilter,                make_shared<PrefilterShader>()},
            {eShaderProgram_BRDF,                     make_shared<BRDFShader>()},
            {eShaderProgram_BG,                       make_shared<BGShader>()},
            {eShaderProgram_PBR,                      make_shared<PBRShader>()},
            {eShaderProgram_DeferredPBR,              make_shared<DeferredPBRShader>()},
            {eShaderProgram_RayTrace,                 make_shared<BVHRayTraceShader>()},
        };
    }
    
    checkGLError();
}

void ShaderManager::checkGLError() {
    GLenum ret = glGetError();
    if(ret != GL_NO_ERROR) {
        printf(">> GL_ERROR: 0x%04x \n", ret);
    }
}

void ShaderManager::unloadAllPrograms() {
    for(auto it = _shaderMap.begin(); it != _shaderMap.end(); it++) {
        it->second->unload();
    }
}

std::shared_ptr<BasicShader> ShaderManager::getActiveShader() const {
    return _activeShader;
}


void ShaderManager::setActiveShader(std::shared_ptr<BasicShader> const & shader) {
    if( _activeShader == shader || shader == nullptr )
        return;
    
    if (!shader->isLoaded()) {
        shader->load();
    }
    _activeShader = shader;
    _activeShader->useProgram();

//    auto & attributes = _activeShader->getEnabledAttributes();
//    for( int i = 0 ; i < VertexAttribute_MAX ; ++i) {
//        if(attributes[i])
//            glEnableVertexAttribArray(i);
//        else
//            glDisableVertexAttribArray(i);
//    }
//    return;
}

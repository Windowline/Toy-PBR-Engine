#ifndef TOYRENDERER_DEPTHDEBUGSHADERTMP_HPP
#define TOYRENDERER_DEPTHDEBUGSHADERTMP_HPP

#include "BasicShader.hpp"

class DepthDebugShaderTmp : public BasicShader {
public:
    DepthDebugShaderTmp();
    bool load() override;
    void useProgram() override;

private:
    void textureUniformLocation() {
        _textureUiformLoc = glGetUniformLocation(_programID, "u_texture");
    }

    GLint _textureUiformLoc = -1;
};

#endif //TOYRENDERER_DEPTHDEBUGSHADERTMP_HPP

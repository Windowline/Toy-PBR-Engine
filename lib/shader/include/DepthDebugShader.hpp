#ifndef TOYRENDERER_DEPTHDEBUGSHADER_HPP
#define TOYRENDERER_DEPTHDEBUGSHADER_HPP

#include "BasicShader.hpp"

class DepthDebugShader : public BasicShader {
public:
    DepthDebugShader();
    bool load() override;
    void useProgram() override;

private:
    void textureUniformLocation() {
        _textureUiformLoc = glGetUniformLocation(_programID, "u_texture");
    }

    GLint _textureUiformLoc = -1;
};

#endif //TOYRENDERER_DEPTHDEBUGSHADER_HPP

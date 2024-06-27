#ifndef TOYRENDERER_SSAOBLURSHADER_HPP
#define TOYRENDERER_SSAOBLURSHADER_HPP

#include "BasicShader.hpp"

class SSAOBlurShader : public BasicShader {
public:
    SSAOBlurShader();
    bool load() override;
    void useProgram() override;

    void textureSizeUniform2f(GLfloat value1, GLfloat value2) {
        assert(_textureSizeUniformLoc != -1);
        glUniform2f(_textureSizeUniformLoc, value1, value2);
    }

private:

    GLint _ssaoTextureUniformLoc = -1;
    GLint _textureSizeUniformLoc = -1;

};

#endif //TOYRENDERER_SSAOBLURSHADER_HPP

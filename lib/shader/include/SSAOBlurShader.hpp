#ifndef TOYRENDERER_SSAOBLURSHADER_HPP
#define TOYRENDERER_SSAOBLURSHADER_HPP

#include "BasicShader.hpp"

class SSAOBlurShader : public BasicShader {
public:
    SSAOBlurShader();
    bool load() override;
    void useProgram() override;


    void ssaoTextureUniform1i(GLuint value) {
        assert(_ssaoTextureUniformLocation != -1);
        glUniform1i(_ssaoTextureUniformLocation, value);
    }

    void textureSizeUniform2f(GLfloat value1, GLfloat value2) {
        assert(_textureSizeUniformLocation != -1);
        glUniform2f(_textureSizeUniformLocation, value1, value2);
    }

private:

    GLint _ssaoTextureUniformLocation = -1;
    GLint _textureSizeUniformLocation = -1;

};

#endif //TOYRENDERER_SSAOBLURSHADER_HPP

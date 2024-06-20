#ifndef TOYRENDERER_SSAOSHADER_HPP
#define TOYRENDERER_SSAOSHADER_HPP

#include "BasicShader.hpp"
#include "Vector.hpp"

class SSAOShader : public BasicShader {
public:
    SSAOShader();
    bool load() override;
    void useProgram() override;

    void posTextureUniform1i(GLuint value) {
        glUniform1i(_posTextureUniformLocation, value);
    }

    void normalTextureUniform1i(GLuint value) {
        glUniform1i(_normalTextureUniformLocation, value);
    }

    void noiseTextureUniform1i(GLuint value) {
        glUniform1i(_noiseTextureUniformLocation, value);
    }

    void samplesUniform3fv(const GLfloat* v, GLsizei count) {
        glUniform3fv(_samplesUniformLocation, count, v);
    }

    void samplesUniformVector(std::vector<vec3>& v) {
        GLfloat* input = new GLfloat[v.size() * 3];
        int idx = 0;
        for (int i = 0; i < v.size(); ++i) {
            input[idx++] = v[i].x;
            input[idx++] = v[i].y;
            input[idx++] = v[i].z;
        }
        samplesUniform3fv(input, GLsizei(v.size()));
    }

    void screenSizeUniform2f(GLfloat value1, GLfloat value2) {
        glUniform2f(_screenSizeUniformLocation, value1, value2);
    }

private:
    GLint _posTextureUniformLocation = -1;
    GLint _normalTextureUniformLocation = -1;
    GLint _noiseTextureUniformLocation = -1;
    GLint _samplesUniformLocation = -1;
    GLint _screenSizeUniformLocation = -1;
};

#endif

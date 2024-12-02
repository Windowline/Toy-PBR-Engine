#ifndef TOYRENDERER_SSAOSHADER_HPP
#define TOYRENDERER_SSAOSHADER_HPP

#include "BasicShader.hpp"
#include "Vector.hpp"
#include <vector>

using namespace std;

class SSAOShader : public BasicShader {
public:
    SSAOShader();
    bool load() override;
    void useProgram() override;

    void samplesUniform3fv(const GLfloat* v, GLsizei count) {
        glUniform3fv(_samplesUniformLoc, count, v);
    }

    void samplesUniformVector(const std::vector<vec3>& v) {
        samplesUniform3fv(&v[0].x, GLsizei(v.size()));
    }

    void screenSizeUniform2f(GLfloat value1, GLfloat value2) {
        glUniform2f(_screenSizeUniformLoc, value1, value2);
    }

private:
    GLint _viewNormalMapUniformLoc = -1;
    GLint _noiseMapUniformLoc = -1;
    GLint _samplesUniformLoc = -1;
    GLint _screenSizeUniformLoc = -1;
};

#endif

#ifndef TOYRENDERER_BVHRAYTRACESHADER_HPP
#define TOYRENDERER_BVHRAYTRACESHADER_HPP

#include "BasicShader.hpp"

class BVHRayTraceShader : public BasicShader {
public:
    BVHRayTraceShader();
    bool load() override;
    void useProgram() override;

    void cameraPosUniform3f(GLfloat value1, GLfloat value2, GLfloat value3) {
        assert(_cameraPosUniformLoc != -1);
        glUniform3f(_cameraPosUniformLoc, value1, value2, value3);
    }

    void cameraLocalToWorldMatUniformMatrix4fv(const GLfloat *value) {
        assert(_cameraLocalToWorldMatUniformLoc != -1);
        glUniformMatrix4fv(_cameraLocalToWorldMatUniformLoc, 1, GL_FALSE, value);
    }

    void resolutionUniform2f(GLfloat value1, GLfloat value2) {
        assert(_resolutionUnifromLoc != -1);
        glUniform2f(_resolutionUnifromLoc, value1, value2);
    }

    void triangleSizeUniform1i(int value) {
        assert(_triangleSizeLoc != -1);
        glUniform1i(_triangleSizeLoc, value);
    }

    void boundsMinUniform3f(GLfloat value1, GLfloat value2, GLfloat value3) {
        assert(_boundsMinLoc != -1);
        glUniform3f(_boundsMinLoc, value1, value2, value3);
    }

    void boundsMaxUniform3f(GLfloat value1, GLfloat value2, GLfloat value3) {
        assert(_boundsMaxLoc != -1);
        glUniform3f(_boundsMaxLoc, value1, value2, value3);
    }


private:
    GLint _cameraPosUniformLoc = -1;
    GLint _cameraLocalToWorldMatUniformLoc = -1;
    GLint _resolutionUnifromLoc = -1;

    GLint _triangleSizeLoc = -1;
    GLint _posTBOLoc = -1;
    GLint _normalTBOLoc = -1;

    GLint _boundsMinLoc = -1;
    GLint _boundsMaxLoc = -1;
};

#endif //TOYRENDERER_BVHRAYTRACESHADER_HPP

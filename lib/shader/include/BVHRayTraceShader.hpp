#ifndef TOYRENDERER_BVHRAYTRACESHADER_HPP
#define TOYRENDERER_BVHRAYTRACESHADER_HPP

#include "BasicShader.hpp"
#include "Vector.hpp"
#include <vector>

using namespace std;

class BVHRayTraceShader : public BasicShader {
public:
    BVHRayTraceShader();
    bool load() override;
    void useProgram() override;

    void cameraPosUniform3f(GLfloat value1, GLfloat value2, GLfloat value3) {
        assert(_cameraPosUniformLoc != -1);
        glUniform3f(_cameraPosUniformLoc, value1, value2, value3);
    }

    void bvhLeafStartIdxUniform1i(int value) {
        assert(_bvhLeafStartIdxLoc != -1);
        glUniform1i(_bvhLeafStartIdxLoc, value);
    }

    void resolutionUniform2f(float x, float y) {
        assert(_resolutionLoc != -1);
        glUniform2f(_resolutionLoc, x, y);
    }

private:
    GLint _cameraPosUniformLoc = -1;
    GLint _bvhNodeTBOLoc = -1;
    GLint _bvhMinBoundsTBOLoc = -1;
    GLint _bvhMaxBoundsTBOLoc = -1;
    GLint _bvhLeafStartIdxLoc = -1;
    GLint _posTBOLoc = -1;
    GLint _normalTBOLoc = -1;

    GLint _resolutionLoc = -1;
};

#endif //TOYRENDERER_BVHRAYTRACESHADER_HPP

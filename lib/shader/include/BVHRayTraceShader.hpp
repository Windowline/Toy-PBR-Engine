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

    void resolutionUniform2f(GLfloat value1, GLfloat value2) {
        assert(_resolutionUnifromLoc != -1);
        glUniform2f(_resolutionUnifromLoc, value1, value2);
    }

    void bvhLeafStartIdxUniform1i(int value) {
        assert(_bvhLeafStartIdxLoc != -1);
        glUniform1i(_bvhLeafStartIdxLoc, value);
    }

    void setRoomColors(const vector<vec3>& roomColors) {
        vector<GLfloat> input;
        for (int i = 0; i < roomColors.size(); ++i) {
            input.push_back(roomColors[i].x);
            input.push_back(roomColors[i].y);
            input.push_back(roomColors[i].z);
        }
        glUniform3fv(_roomFaceColorsLoc, GLsizei(roomColors.size()), input.data());
    }

private:
    GLint _cameraPosUniformLoc = -1;
    GLint _resolutionUnifromLoc = -1;
    GLint _roomFaceColorsLoc = -1;

    GLint _bvhNodeTBOLoc = -1;
    GLint _bvhMinBoundsTBOLoc = -1;
    GLint _bvhMaxBoundsTBOLoc = -1;
    GLint _bvhLeafStartIdxLoc = -1;
    GLint _posTBOLoc = -1;
    GLint _normalTBOLoc = -1;
    GLint _roomPosTBOLoc = -1;
    GLint _roomNormalTBOLoc = -1;
};

#endif //TOYRENDERER_BVHRAYTRACESHADER_HPP

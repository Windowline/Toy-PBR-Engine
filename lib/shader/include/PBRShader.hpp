#ifndef TOYRENDERER_PBRSHADER_H
#define TOYRENDERER_PBRSHADER_H

#include "BasicShader.hpp"

class PBRShader : public BasicShader {
public:
    PBRShader();
    bool load() override;
    void useProgram() override;

    void albedoUniform3f(GLfloat value1, GLfloat value2, GLfloat value3) {
        assert(_albedoLoc != -1);
        glUniform3f(_albedoLoc, value1, value2, value3);
    }

    void metallicUniform1f(GLfloat v) {
        assert(_metallicLoc != -1);
        glUniform1f(_metallicLoc, v);
    }

    void roughnessUniform1f(GLfloat v) {
        assert(_roughnessLoc != -1);
        glUniform1f(_roughnessLoc, v);
    }

    void aoUniform1f(GLfloat v) {
        assert(_aoLoc != -1);
        glUniform1f(_aoLoc, v);
    }

    void lightPositionsUniformVec3fv(const GLfloat* v, int count) {
        assert(_lightPositionsLoc != -1);
        glUniform3fv(_lightPositionsLoc, count, v);
    }

    void lightColorsUniformVec3fv(const GLfloat* v, int count) {
        assert(_lightColorsLoc != -1);
        glUniform3fv(_lightColorsLoc, count, v);
    }

    void camPosUniform3f(GLfloat value1, GLfloat value2, GLfloat value3) {
        assert(_camPosLoc != -1);
        glUniform3f(_camPosLoc, value1, value2, value3);
    }



private:

    GLint _albedoLoc = -1;
    GLint _metallicLoc = -1;
    GLint _roughnessLoc = -1;
    GLint _aoLoc = -1;

    GLint _lightPositionsLoc = -1;
    GLint _lightColorsLoc = -1;
    GLint _camPosLoc = -1;

    GLint _irradianceMapLoc = -1;
    GLint _prefilterMapLoc = -1;
    GLint _brdfLUTLoc = -1;
};

#endif //TOYRENDERER_PBRSHADER_H

#ifndef TOYRENDERER_GBUFFERSHADER_HPP
#define TOYRENDERER_GBUFFERSHADER_HPP

#include "BasicShader.hpp"

class GBufferShader : public BasicShader {
public:
    GBufferShader();
    bool load() override;
    void useProgram() override;

    void isRenderSkyBokxUniform1f(GLfloat value) {
        assert(_isRenderSkyBoxLoc != -1);
        glUniform1f(_isRenderSkyBoxLoc, value);
    }

    void colorUniform3f(GLfloat value1, GLfloat value2, GLfloat value3) {
        assert(_colorLoc != -1);
        glUniform3f(_colorLoc, value1, value2, value3);
    }

    void colorUniform3fv(const GLfloat* value, int count) {
        assert(_colorLoc != -1);
//        glUniform3fv(_colorLoc, value1, value2, value3);
        glUniform3fv(_colorLoc, count, value);
    }



private:

    GLint _isRenderSkyBoxLoc = -1;
    GLint _colorLoc = -1;
    GLint _envCubeMapLoc = -1;

};

#endif //TOYRENDERER_GBUFFERSHADER_HPP

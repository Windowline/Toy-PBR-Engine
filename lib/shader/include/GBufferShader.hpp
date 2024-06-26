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

private:

    GLint _isRenderSkyBoxLoc = -1;
    GLint _envCubeMapLoc = -1;

};

#endif //TOYRENDERER_GBUFFERSHADER_HPP

#ifndef TOYRENDERER_GBUFFERSHADERTMP_HPP
#define TOYRENDERER_GBUFFERSHADERTMP_HPP

#include "BasicShader.hpp"

class GBufferShaderTmp : public BasicShader {
public:
    GBufferShaderTmp();
    bool load() override;

    void worldMatUniformMatrix4fv(const GLfloat *value) {
        glUniformMatrix4fv(_worldMatUniformLocation, 1, GL_FALSE, value);
    }

private:
    void worldUniformLocation() {
        _worldMatUniformLocation = glGetUniformLocation(_programID, "u_worldMat");
    }



    GLint _worldMatUniformLocation;
    GLint _worldViewMatUniformLocation;
    GLint _projMatUniformLocation;

};

#endif //TOYRENDERER_GBUFFERSHADERTMP_HPP

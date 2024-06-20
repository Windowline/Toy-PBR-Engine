#ifndef TOYRENDERER_GBUFFERSHADER_HPP
#define TOYRENDERER_GBUFFERSHADER_HPP

#include "BasicShader.hpp"

class GBufferShader : public BasicShader {
public:
    GBufferShader();
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

#endif //TOYRENDERER_GBUFFERSHADER_HPP

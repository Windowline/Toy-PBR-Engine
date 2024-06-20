#ifndef TOYRENDERER_TEXTUREPASSSHADER_HPP
#define TOYRENDERER_TEXTUREPASSSHADER_HPP

#include "BasicShader.hpp"

class TexturePassShader : public BasicShader {
public:
    TexturePassShader();
    bool load() override;
    void useProgram() override;

private:
    void textureUniformLocation() {
        _textureUiformLoc = glGetUniformLocation(_programID, "u_texture");
    }

    GLint _textureUiformLoc = -1;
};

#endif //TOYRENDERER_TEXTUREPASSSHADER_HPP

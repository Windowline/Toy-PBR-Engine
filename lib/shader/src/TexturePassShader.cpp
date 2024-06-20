#include "TexturePassShader.hpp"

const char* vertexTexturePassShaderTmp = R(
        layout (location = 0) in vec2 a_position;
        layout (location = 1) in vec2 a_texCoord;

        out vec2 v_texCoord;

        void main()
        {
           v_texCoord = a_texCoord;
           gl_Position = vec4(a_position, 0.0, 1.0);
        }
);

const char* fragmentTexturePassShaderTmp = R(
        uniform sampler2D u_texture;
        in vec2 v_texCoord;
        layout (location = 0) out vec4 fragColor;

        void main()
        {
            fragColor = vec4(texture(u_texture, v_texCoord).rgb, 1.0);
        }
);

TexturePassShader::TexturePassShader() {
    this->load();
    textureUniformLocation();
}

bool TexturePassShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexTexturePassShaderTmp);
    string fShader = string("#version 330 core \n") + string(fragmentTexturePassShaderTmp);

    _programID = loadProgram_tmp(reinterpret_cast<const char *>(vShader.c_str()),
                                 reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}

void TexturePassShader::useProgram() {
    glUseProgram(_programID);

    assert(_textureUiformLoc != -1);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(_textureUiformLoc, 0);
}
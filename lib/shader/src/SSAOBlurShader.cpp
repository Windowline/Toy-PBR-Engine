#include "SSAOBlurShader.hpp"

const char* vertexSSAOBlur = R(
        layout (location = 0) in vec2 a_position;
        layout (location = 1) in vec2 a_texCoord;
        out vec2 v_texCoord;

        void main() {
           v_texCoord = a_texCoord;
           gl_Position = vec4(a_position, 0.0, 1.0);
        }
);

const char* fragmentSSAOBlur = R(
        layout (location = 0) out vec4 fragColor; //todo: float

        in vec2 v_texCoord;

        uniform sampler2D u_ssaoTexture;
        uniform vec2 u_textureSize;

        void main() {
           vec2 texelSize = 1.0 / u_textureSize;
           float result = 0.0;
           for (int x = -4; x < 4; ++x) {
              for (int y = -4; y < 4; ++y) {
                 vec2 offset = vec2(float(x), float(y)) * texelSize;
                 result += texture(u_ssaoTexture, v_texCoord + offset).r;
               }
            }
            fragColor = vec4(vec3(result / 64.0), 1.0);
        }
);

SSAOBlurShader::SSAOBlurShader() {
    this->load();
    basicUniformLoc();
    _ssaoTextureUniformLoc = glGetUniformLocation(_programID, "u_ssaoTexture");
    _textureSizeUniformLoc = glGetUniformLocation(_programID, "u_textureSize");
}

bool SSAOBlurShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexSSAOBlur);
    string fShader = string("#version 330 core \n") + string(fragmentSSAOBlur);

    _programID = loadProgram(reinterpret_cast<const char *>(vShader.c_str()),
                             reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}

void SSAOBlurShader::useProgram() {
    glUseProgram(_programID);

    assert(_ssaoTextureUniformLoc != -1);
    glUniform1i(_ssaoTextureUniformLoc, 0);
}
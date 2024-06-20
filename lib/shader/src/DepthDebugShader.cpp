#include "DepthDebugShader.hpp"

const char* vertexDepthDebugTmp = R(
        layout (location = 0) in vec2 a_position;
        layout (location = 1) in vec2 a_texCoord;

        out vec2 v_texCoord;

        void main()
        {
            v_texCoord = a_texCoord;
            gl_Position = vec4(a_position, 0.0, 1.0);
        }
);

const char* fragmentDepthDebugTmp = R(
        layout (location = 0) out vec4 fragColor;
        in vec2 v_texCoord;

        uniform sampler2D u_texture;
        uniform float near_plane;
        uniform float far_plane;

        // required when using a perspective projection matrix
        float LinearizeDepth(float depth)
        {
            float z = depth * 2.0 - 1.0; // Back to NDC
            return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
        }

        void main()
        {
            fragColor = vec4(vec3(texture(u_texture, v_texCoord).r), 1.0);
        }
);

DepthDebugShader::DepthDebugShader() {
    this->load();
    basicUniformLoc();
    textureUniformLocation();
}

bool DepthDebugShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexDepthDebugTmp);
    string fShader = string("#version 330 core \n") + string(fragmentDepthDebugTmp);

    _programID = loadProgram_tmp(reinterpret_cast<const char *>(vShader.c_str()),
                                 reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}

void DepthDebugShader::useProgram() {
    glUseProgram(_programID);

    assert(_textureUiformLoc != -1);
    glUniform1i(_textureUiformLoc, 0);
}
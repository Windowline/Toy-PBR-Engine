#include "BGShader.hpp"

const char* vertexBG = R(
        layout (location = 0) in vec3 aPos;
        uniform mat4 u_projMat;
        uniform mat4 u_viewMat;

        out vec3 WorldPos;

        void main() {
            WorldPos = aPos;

            mat4 rotView = mat4(mat3(u_viewMat));
            vec4 clipPos = u_projMat * rotView * vec4(WorldPos, 1.0);

            gl_Position = clipPos.xyww;
        }
);

const char* fragmentBG = R(
        out vec4 FragColor;
        in vec3 WorldPos;

        uniform samplerCube u_environmentMap;

        void main() {
            vec3 envColor = textureLod(u_environmentMap, WorldPos, 0.0).rgb;

            // HDR tonemap and gamma correct
            envColor = envColor / (envColor + vec3(1.0));
            envColor = pow(envColor, vec3(1.0/2.2));

            FragColor = vec4(envColor, 1.0);
        }
);


BGShader::BGShader() {
    this->load();
    basicUniformLoc();
    _equirectangularMapLoc =  glGetUniformLocation(_programID, "u_environmentMap");
}

bool BGShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexBG);
    string fShader = string("#version 330 core \n") + string(fragmentBG);

    _programID = loadProgram(reinterpret_cast<const char *>(vShader.c_str()),
                             reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}

void BGShader::useProgram() {
    glUseProgram(_programID);
    assert(_equirectangularMapLoc != -1);
    glUniform1i(_equirectangularMapLoc, 0);
}

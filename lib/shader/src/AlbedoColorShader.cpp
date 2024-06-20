#include "AlbedoColorShader.hpp"

const char* vertexAlbedoShader = R(
        layout (location = 0) in vec3 a_position;
        layout (location = 1) in vec3 a_color;
        layout (location = 2) in vec3 a_normal;
        out vec3 v_color;

        void main()
        {
           v_color = a_color;
           gl_Position = vec4(a_position, 1.0);
        }
);

const char* fragmentAlbedoShader = R(
        in vec3 v_color;
        out vec4 FragColor;
        void main()
        {
           FragColor = vec4(v_color, 1.0f);
        }
);

AlbedoColorShader::AlbedoColorShader() {
    this->load();
}

bool AlbedoColorShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexAlbedoShader);
    string fShader = string("#version 330 core \n") + string(fragmentAlbedoShader);


    _programID = loadProgram_tmp(reinterpret_cast<const char *>(vShader.c_str()),
                                 reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}

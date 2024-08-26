#include "SimpleShader.hpp"

const char* vertexSimple = R(
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;

        uniform mat4 u_projMat;
        uniform mat4 u_viewMat;
        out vec3 v_color;

        void main() {
            v_color = aColor;
            gl_Position = u_projMat * u_viewMat * vec4(aPos, 1.0);
        }
);

const char* fragmentSimple = R(
        out vec4 FragColor;
        uniform vec3 u_color;
        in vec3 v_color;

        void main() {
            vec3 tmp = u_color;
//            FragColor = vec4(u_color, 1.0);
            FragColor = vec4(v_color.rgb, 1.0f);
        }
);


SimpleShader::SimpleShader() {
    this->load();
    basicUniformLoc();
    _colorLoc =  glGetUniformLocation(_programID, "u_color");
}

bool SimpleShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexSimple);
    string fShader = string("#version 330 core \n") + string(fragmentSimple);

    _programID = loadProgram(reinterpret_cast<const char *>(vShader.c_str()),
                             reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}

void SimpleShader::useProgram() {
    glUseProgram(_programID);
}

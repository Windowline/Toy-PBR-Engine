#include "TestShader.hpp"

const char *vShader = "#version 330 core\n"
//                                 "layout (location = 0) in vec3 a_position;\n"
                                 "in vec3 a_position;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(a_position.x, a_position.y, a_position.z, 1.0);\n"
                                 "}\0";
const char *fShader = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);\n"
                                   "}\n\0";


TestShader::TestShader() : ShaderBase() {

}

bool TestShader::load() {

    _programID = loadProgram(reinterpret_cast<const char *>(vShader),
                             reinterpret_cast<const char *>(fShader));

    if (_programID == INVALID_GL_ID) {
        return false;
    }

    positionAttribLocation();
    return true;
}

void TestShader::useProgram() {
    ShaderBase::useProgram();
    glEnableVertexAttribArray(_positionAttribLocation);
}
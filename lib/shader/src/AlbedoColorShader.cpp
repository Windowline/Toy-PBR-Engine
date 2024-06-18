#include "AlbedoColorShader.hpp"

static GLbyte vShader[] =
"#version 330 core                                                            \n"
"layout (location = 0) in vec3 a_position;                                                          \n"
"layout (location = 1) in vec3 a_color;                                                             \n"
"layout (location = 2) in vec3 a_normal;                                                            \n"

"uniform mat4 u_mvp;                                                        \n"

"out vec3 v_color;                                                          \n"

"void main(void)                                                            \n"
"{                                                                          \n"
"   v_color = a_color;                                                      \n"
"   gl_Position = u_mvp * vec4(a_position, 1.0);                            \n"
"}                                                                          \n";

static GLbyte fShader[] =
"#version 330 core                                                            \n"
"in vec3 v_color;                                                           \n"
"layout (location = 0) out vec4 fragColor;                                  \n"
                                                                
"void main(void)                                                            \n"
"{                                                                          \n"
"   fragColor = vec4(v_color, 1.0);                                 \n"
"}                                                                          \n";




AlbedoColorShader::AlbedoColorShader() : ShaderBase() {
    _mvpUniformLocation = Shader_Invalid_Location;
}


bool AlbedoColorShader::load() {
    
    _programID = loadProgram(reinterpret_cast<const char *>(vShader),
                             reinterpret_cast<const char *>(fShader));
    
    if (_programID == INVALID_GL_ID) {
        return false;
    }
    
    positionAttribLocation();
    colorAttribLocation();
    mvpUniformLocation();
    
    return true;
}

void AlbedoColorShader::useProgram() {
    ShaderBase::useProgram();
    glEnableVertexAttribArray(_positionAttribLocation);
    glEnableVertexAttribArray(_colorAttribLocation);
}

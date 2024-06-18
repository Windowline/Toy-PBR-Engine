#include "AlbedoColorShader.hpp"

static GLbyte vAlbedoShader[] =
"#version 330 core                                                            \n"
"layout (location = 0) in vec3 a_position;                                                          \n"
"layout (location = 1) in vec3 a_color;                                                             \n"
"layout (location = 2) in vec3 a_normal;                                                            \n"

//"uniform mat4 u_mvp;                                                        \n"

"out vec3 v_color;                                                          \n"

"void main()                                                            \n"
"{                                                                          \n"
"   v_color = a_color;                                                      \n"
"   gl_Position = vec4(a_position, 1.0);                            \n"
"}                                                                          \n";

static GLbyte fAlbedoShader[] =
"#version 330 core                                                            \n"
"in vec3 v_color;                                                           \n"
"layout (location = 0) out vec4 fragColor;                                  \n"

"void main()                                                            \n"
"{                                                                          \n"
"   fragColor = vec4(v_color, 1.0);                                         \n"
"}                                                                          \n";

//const char *vAlbedoShader = "#version 330 core\n"
//                                  "layout (location = 0) in vec3 a_position; \n"
//                                  "layout (location = 1) in vec3 a_color;    \n"
//                                  "layout (location = 2) in vec3 a_normal;    \n"
//
//                                  "out vec3 v_color;                         \n"
//                                  "void main()\n"
//                                  "{\n"
//                                  "   v_color = a_color;                     \n"
//                                  "   gl_Position = vec4(a_position.x, a_position.y, a_position.z, 1.0);\n"
//                                  "}\0";
//const char *fAlbedoShader = "#version 330 core       \n"
//                                    "in vec3 v_color;        \n"
//                                    "out vec4 FragColor;     \n"
//                                    "void main()             \n"
//                                    "{                                           \n"
//                                    "   FragColor = vec4(v_color.x, v_color.y, v_color.z, 1.0f);\n"
//                                    "}\n\0";




AlbedoColorShader::AlbedoColorShader() : ShaderBase() {
    _mvpUniformLocation = Shader_Invalid_Location;
    this->load();
}


bool AlbedoColorShader::load() {
    
    _programID = loadProgram(reinterpret_cast<const char *>(vAlbedoShader),
                             reinterpret_cast<const char *>(fAlbedoShader));
    
    if (_programID == INVALID_GL_ID) {
        return false;
    }
    
//    positionAttribLocation();
//    colorAttribLocation();
    mvpUniformLocation();
    
    return true;
}

void AlbedoColorShader::useProgram() {
    glUseProgram(_programID);
//    ShaderBase::useProgram();
//    glEnableVertexAttribArray(0);
//    glEnableVertexAttribArray(1);
//    glEnableVertexAttribArray(2);
}

#include "../include/GBufferShader.hpp"
static GLbyte vShader[] =

"#version 300 es                                                        \n"
"precision highp float;                                                 \n"

"uniform mat4 u_worldMat;                                               \n"
"uniform mat4 u_worldViewMat;                                           \n"
"uniform mat4 u_projMat;                                                \n"
"uniform mat4 u_worldNormalMat;                                         \n"

"in vec3 a_position;                                                    \n"
"in vec3 a_normal;                                                      \n"
"in vec3 a_color;                                                       \n"

"out vec3 v_color;                                                      \n"
"out vec3 v_position;                                                   \n"
"out vec3 v_normal;                                                     \n"

"                                                                           \n"
"void main(void)                                                            \n"
"{                                                                          \n"
"   vec4 worldPos = u_worldMat * vec4(a_position, 1.0);                     \n"
"   v_normal = normalize((u_worldNormalMat * vec4(a_normal, 0.0)).xyz);     \n"

"   v_color = a_color;                                                                  \n"
"   v_position = worldPos.xyz;                                                          \n"
"   gl_Position = u_projMat * u_worldViewMat * vec4(a_position, 1.0);       \n"
"}                                                                          \n";

static GLbyte fShader[] =

"#version 300 es                                                          \n"
"precision highp float;                                                   \n"

"in vec3 v_position;                                                                \n"
"in vec3 v_color;                                                                   \n"
"in vec3 v_normal;                                                                  \n"

"layout (location = 0) out vec4 gPosition;                                          \n"
"layout (location = 1) out vec4 gNormal;                                            \n"
"layout (location = 2) out vec4 gAlbedo;                                            \n"

"void main(void)                                                                    \n"
"{                                                                                  \n"
"    gPosition = vec4(v_position, 1.0);                                             \n"
"    gNormal = vec4(normalize(v_normal), 1.0);                                       \n"
"    gAlbedo = vec4(v_color, 1.0);                                                   \n"
"}                                                                                  \n";


GBufferShader::GBufferShader() : BasicLightingShader() {
}


bool GBufferShader::load() {
    _programID = loadProgram(reinterpret_cast<const char *>(vShader),
                             reinterpret_cast<const char *>(fShader));
    
    if (_programID == 0) {
        return false;
    }
    
    //attrib
    positionAttribLocation();
    colorAttribLocation();
    normalAttribLocation();
    
    //uniform
    worldUniformLocation();
    worldViewUniformLocation();
    projUniformLocation();
    worldNormalMatUniformLocation();

    return true;
}


void GBufferShader::useProgram() {
    ShaderBase::useProgram();
    glEnableVertexAttribArray(_positionAttribLocation);
    glEnableVertexAttribArray(_colorAttribLocation);
    glEnableVertexAttribArray(_normalAttribLocation);
}



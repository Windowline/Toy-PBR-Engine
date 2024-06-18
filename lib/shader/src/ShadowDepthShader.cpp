#include "../include/ShadowDepthShader.hpp"


static GLbyte vShader[] =
"#version 330 core                                                          \n"
"precision highp float;                                                   \n"

"uniform mat4 u_worldViewMat;                                              \n"
"uniform mat4 u_projMat;                                                   \n"
"uniform mat4 u_shadowMVP;                                                 \n"

"in vec3 a_position;                                                       \n"
"in vec3 a_color;                                                           \n"
"out vec4 v_shadowClipPos;                                                 \n"
"out vec3 v_color;  \n"

"void main(void)                                                           \n"
"{                                                                         \n"
"   v_shadowClipPos = u_shadowMVP * vec4(a_position, 1.0);                  \n"
"   v_color = a_color;  \n"
"   gl_Position = u_shadowMVP * vec4(a_position, 1.0);                      \n"
"}                                                                          \n";

static GLbyte fShader[] =
"#version 330 core                                                                \n"
"precision highp float;                                                         \n"

"in vec4 v_shadowClipPos;                                                           \n"
"in vec3 v_color;   \n"

"layout (location = 0) out vec4 shadowDepth;                                        \n"

"void main(void)                                                                    \n"
"{                                                                                  \n"
"    shadowDepth = vec4(vec3(v_shadowClipPos.z / v_shadowClipPos.w), 1.0);          \n"
"}                                                                                  \n";


ShadowDepthShader::ShadowDepthShader() : BasicLightingShader() {
  
}

bool ShadowDepthShader::load() {
    
    _programID = loadProgram(reinterpret_cast<const char *>(vShader),
                             reinterpret_cast<const char *>(fShader));
    
    if (_programID == INVALID_GL_ID) {
        return false;
    }
    
    positionAttribLocation();
    colorAttribLocation();
//    normalAttribLocation();
//
    worldViewUniformLocation();
    projUniformLocation();
    
    shadowMVPUniformLocation();
    
    return true;
}

void ShadowDepthShader::useProgram() {
    BasicLightingShader::useProgram();
    glEnableVertexAttribArray(_positionAttribLocation);
    glEnableVertexAttribArray(_colorAttribLocation);
    glEnableVertexAttribArray(_normalAttribLocation);
}


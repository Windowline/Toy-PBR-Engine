#include "../include/SSAOBlurShader.hpp"
#include "GLUtilGeometry.hpp"

static GLbyte vShader[] =
"#version 300 es                                                            \n"
"precision highp float;                                                     \n"

"in vec2 a_position;                                                        \n"
"in vec2 a_texCoord;                                                        \n"
"out vec2 v_texCoord;                                                       \n"
"                                                                           \n"
"void main(void)                                                            \n"
"{                                                                          \n"
"   v_texCoord = a_texCoord;                                                \n"
"   gl_Position = vec4(a_position, 0.0, 1.0);                               \n"
"}                                                                          \n";

static GLbyte fShader[] =
"#version 300 es                                                            \n"
"precision highp float;                                                     \n"

"in vec2 v_texCoord;                                                        \n"
"uniform sampler2D u_ssaoTexture;                                           \n"
"uniform vec2 u_textureSize;                                                \n"

"layout (location = 0) out vec4 fragColor;                                  \n" //todo: float

"void main() {                                                              \n"
"   vec2 texelSize = 1.0 / u_textureSize;                                     \n"
"   float result = 0.0;                                                     \n"
"   for (int x = -4; x < 4; ++x) {                                          \n"
"      for (int y = -4; y < 4; ++y) {                                       \n"
"         vec2 offset = vec2(float(x), float(y)) * texelSize;               \n"
"         result += texture(u_ssaoTexture, v_texCoord + offset).r;          \n"
"       }                                                                   \n"
"    }                                                                      \n"
"    fragColor = vec4(vec3(result / 64.0), 1.0);                            \n"
"}                                                                          \n";

SSAOBlurShader::SSAOBlurShader() : ShaderBase() {
    
    _texCoordAttribLocation = Shader_Invalid_Location;
    _ssaoTextureUniformLocation = Shader_Invalid_Location;
    _textureSizeUniformLocation = Shader_Invalid_Location;
}



bool SSAOBlurShader::load() {
    _programID = loadProgram(reinterpret_cast<const char *>(vShader),
                             reinterpret_cast<const char *>(fShader));
    
    if (_programID == INVALID_GL_ID) {
        return false;
    }
    
    //attrib
    positionAttribLocation();
    texCoordAttribLocation();
    
    //uniform
    ssaoTextureUniformLocation();
    textureSizeUniformLocation();
    
    return true;
}

void SSAOBlurShader::useProgram() {
    ShaderBase::useProgram();
    glEnableVertexAttribArray(_positionAttribLocation);
    glEnableVertexAttribArray(_texCoordAttribLocation);
    
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(_ssaoTextureUniformLocation, 0);
    
}

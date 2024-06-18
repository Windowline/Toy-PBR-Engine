
#include "../include/TexturePassShader.hpp"


static GLbyte vShader[] =
"#version 330 core                                                            \n"
"precision highp float;                                                     \n"

"in vec2 a_position;                                                        \n"
"in vec2 a_texCoord;                                                         \n"
"out vec2 v_texCoord;                                                       \n"

"void main(void)                                                            \n"
"{                                                                          \n"
"   v_texCoord = a_texCoord;                                                \n"
"   gl_Position = vec4(a_position, 0.0, 1.0);                               \n"
"}                                                                          \n";

static GLbyte fShader[] =
"#version 330 core                                                            \n"
"precision highp float;                                                     \n"

"uniform sampler2D u_texture;                                               \n"
"in vec2 v_texCoord;                                                        \n"
"layout (location = 0) out vec4 fragColor;                                  \n"
                                                                
"void main(void)                                                            \n"
"{                                                                          \n"
"    fragColor = vec4(texture(u_texture, v_texCoord).rgb, 1.0);             \n"
"}                                                                          \n";



TexturePassShader::TexturePassShader() : ShaderBase() {
    
    _texCoordAttribLocation = Shader_Invalid_Location;
    
}

bool TexturePassShader::load() {
    
    _programID = loadProgram(reinterpret_cast<const char *>(vShader),
                             reinterpret_cast<const char *>(fShader));
    
    if (_programID == INVALID_GL_ID) {
        return false;
    }
    
    positionAttribLocation();
    texCoordAttribLocation();
    
    textureUniformLocation();
    
    return true;
}


void TexturePassShader::useProgram() {
    ShaderBase::useProgram();
    glEnableVertexAttribArray(_positionAttribLocation);
    glEnableVertexAttribArray(_texCoordAttribLocation);
    
    glActiveTexture(GL_TEXTURE0);
    textureUniform1i(0);
}

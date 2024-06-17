
#ifndef TexturePassShader_hpp
#define TexturePassShader_hpp

#include "ShaderBase.hpp"

/**
 * 디버깅을 위한 fullquad를 그리는 셰이더입니다.
 */
class TexturePassShader : public ShaderBase {
public:
    TexturePassShader();
    
    virtual bool load() override;
    virtual void useProgram() override;
    
    
    /* TextureCoord */
    void texCoordAttribLocation() {
        _texCoordAttribLocation = glGetAttribLocation(_programID, "a_texCoord");
        assert(_texCoordAttribLocation != Shader_Invalid_Location);
    }
    
    void texCoordAttribPointer(const GLvoid *ptr, int nChannels) {
        if (!_enabledAttributes[_texCoordAttribLocation]) {
            _enabledAttributes[_texCoordAttribLocation] = true;
            glEnableVertexAttribArray(_texCoordAttribLocation);
        }
        vertexAttribPointer(_texCoordAttribLocation, nChannels, GL_FLOAT, GL_FALSE, 0, ptr);
    }
    
    
    void textureUniformLocation() {
        _textureUniformLocation = glGetUniformLocation(_programID, "u_texture");
    }
    
    void textureUniform1i(GLuint value) {
        glUniform1i(_textureUniformLocation, value);
    }
    
private:
    GLint _texCoordAttribLocation;
    GLint _textureUniformLocation;
};


#endif /* TexturePassShader_hpp */

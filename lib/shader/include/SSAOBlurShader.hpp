#ifndef SSAOBlurShader_hpp
#define SSAOBlurShader_hpp

#include "ShaderBase.hpp"
#include "Vector.hpp"


class SSAOBlurShader : public ShaderBase {

public:
    SSAOBlurShader();
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
   
    void ssaoTextureUniformLocation() {
        _ssaoTextureUniformLocation = glGetUniformLocation(_programID, "u_ssaoTexture");
    }
    
    void ssaoTextureUniform1i(GLuint value) {
        glUniform1i(_ssaoTextureUniformLocation, value);
    }
    
    void textureSizeUniformLocation() {
        _textureSizeUniformLocation = glGetUniformLocation(_programID, "u_textureSize");
    }
    
    void textureSizeUniform2f(GLfloat value1, GLfloat value2) {
        glUniform2f(_textureSizeUniformLocation, value1, value2);
    }
    
    
protected:
    
    GLint _texCoordAttribLocation;
    GLint _ssaoTextureUniformLocation;
    GLint _textureSizeUniformLocation;
};


#endif /* SSAOBlurShader_hpp */

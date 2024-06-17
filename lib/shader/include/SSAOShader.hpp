#ifndef SSAOShader_hpp
#define SSAOShader_hpp

#include "ShaderBase.hpp"
#include "Vector.hpp"

/**
 * position, normal, 
 * Screen Space Ambient Occlusion를 그립니다.
 * 추후 디퍼드 렌더링에 이용됩니다.
 */
class SSAOShader : public ShaderBase {

public:
    SSAOShader();
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
    
    
    void posTextureUniformLocation() {
        _posTextureUniformLocation = glGetUniformLocation(_programID, "u_posTexture");
    }
    
    void posTextureUniform1i(GLuint value) {
        glUniform1i(_posTextureUniformLocation, value);
    }
    
    
    void normalTextureUniformLocation() {
        _normalTextureUniformLocation = glGetUniformLocation(_programID, "u_normalTexture");
    }
    
    void normalTextureUniform1i(GLuint value) {
        glUniform1i(_normalTextureUniformLocation, value);
    }
    
    
    void noiseTextureUniformLocation() {
        _noiseTextureUniformLocation = glGetUniformLocation(_programID, "u_noiseTexture");
    }
    
    void noiseTextureUniform1i(GLuint value) {
        glUniform1i(_noiseTextureUniformLocation, value);
    }
    
    
    //noise sample 64 array
    void samplesUniformLocation() {
        _samplesUniformLocation = glGetUniformLocation(_programID, "u_samples");
    }
    
    void samplesUniform3fv(const GLfloat* v, GLsizei count) {
        glUniform3fv(_samplesUniformLocation, count, v);
    }
    
    void samplesUniformVector(std::vector<vec3>& v) {
        GLfloat* input = new GLfloat[v.size() * 3];
        int idx = 0;
        for (int i = 0; i < v.size(); ++i) {
            input[idx++] = v[i].x;
            input[idx++] = v[i].y;
            input[idx++] = v[i].z;
        }
        samplesUniform3fv(input, GLsizei(v.size()));
    }
    
    //screen size
    void screenSizeUniformLocation() {
        _screenSizeUniformLocation = glGetUniformLocation(_programID, "u_screenSize");
    }
    
    void screenSizeUniform2f(GLfloat value1, GLfloat value2) {
        glUniform2f(_screenSizeUniformLocation, value1, value2);
    }
    
    //view
    void viewMatUniformLocation() {
        _projMatUniformLocation = glGetUniformLocation(_programID, "u_viewMat");
    }
    
    void viewMatUniformMatrix4fv(const GLfloat *value) {
        glUniformMatrix4fv(_viewMatUniformLocation, 1, GL_FALSE, value);
    }
    
    //proj
    void projMatUniformLocation() {
        _projMatUniformLocation = glGetUniformLocation(_programID, "u_projMat");
    }
    
    void projMatUniformMatrix4fv(const GLfloat *value) {
        glUniformMatrix4fv(_projMatUniformLocation, 1, GL_FALSE, value);
    }
    
    
protected:
    
    GLint _texCoordAttribLocation;
    
    GLint _posTextureUniformLocation;
    GLint _normalTextureUniformLocation;
    GLint _noiseTextureUniformLocation;
    
    GLint _samplesUniformLocation;
    
    GLint _screenSizeUniformLocation;
    
    GLint _viewMatUniformLocation;
    GLint _projMatUniformLocation;
    
};


#endif /* SSAOShader_hpp */

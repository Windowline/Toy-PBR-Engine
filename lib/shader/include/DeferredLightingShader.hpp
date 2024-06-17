

#ifndef DeferredLightingShader_hpp
#define DeferredLightingShader_hpp

#include "ShaderBase.hpp"
#include "Vector.hpp"

/**
 * gBuffer(position, normal, albedo), shadow depth buffer, blur-ssao buffer를
 * 입력으로 받아서 Deferred Lighting/Shadow를 적용합니다. (Full Quad)
 */
class DeferredLightingShader : public ShaderBase {

public:
    DeferredLightingShader();
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
    
    
    void albedoTextureUniformLocation() {
        _albedoTextureUniformLocation = glGetUniformLocation(_programID, "u_albedoTexture");
    }
    
    void albedoTextureUniform1i(GLuint value) {
        glUniform1i(_albedoTextureUniformLocation, value);
    }
    
    void shadowDepthUniformLocation() {
        _shadowDepthUniformLocation = glGetUniformLocation(_programID, "u_shadowDepth");
    }
    
    void shadowDepthUniform1i(GLuint value) {
        glUniform1i(_shadowDepthUniformLocation, value);
    }
    
    
    //light pos
    void worldLightPosUniformLocation() {
        _worldLightPosUniformLocation = glGetUniformLocation(_programID, "u_worldLightPos");
    }
    
    void worldLightPosUniform3fv(const GLfloat* v, GLsizei count) {
        glUniform3fv(_worldLightPosUniformLocation, count, v);
    }
    
    void worldLightPosUniform3fVector(const std::vector<vec3>& lightPositions) {
        const int LIGHT_COMPONENT_COUNT = 3;
        float* lightPosArray = new float[lightPositions.size() * LIGHT_COMPONENT_COUNT];
        int idx = 0;
        for (auto& lightPos : lightPositions) {
            lightPosArray[idx++] = lightPos.x;
            lightPosArray[idx++] = lightPos.y;
            lightPosArray[idx++] = lightPos.z;
        }
        
        worldLightPosUniform3fv(lightPosArray, (int)lightPositions.size());
        lightCountUniform1i((int)lightPositions.size());
        
        delete [] lightPosArray;
    }
    
    void lightCountUniformLocation() {
        _lightCountUniformLocation = glGetUniformLocation(_programID, "u_lightCount");
    }
    
    void lightCountUniform1i(GLint value) {
        glUniform1i(_lightCountUniformLocation, value);
    }
    
    
    //diffuse color
    void diffuseColorUniformLocation() {
        _diffuseColorUniformLocation = glGetUniformLocation(_programID, "u_diffuseColor");
    }
    
    void diffuseColorUniform3f(GLfloat value1, GLfloat value2, GLfloat value3) {
        glUniform3f(_diffuseColorUniformLocation, value1, value2, value3);
    }
    
    //specular color
    void specularColorUniformLocation() {
        _specularColorUniformLocation = glGetUniformLocation(_programID, "u_specularColor");
    }
    
    void specularColorUniform3f(GLfloat value1, GLfloat value2, GLfloat value3) {
        glUniform3f(_specularColorUniformLocation, value1, value2, value3);
    }
    
    
    //ambient
    void ambientColorUniformLocation() {
        _ambientColorUniformLocation = glGetUniformLocation(_programID, "u_ambientColor");
    }
    
    void ambientColorUniform3f(GLfloat value1, GLfloat value2, GLfloat value3) {
        glUniform3f(_ambientColorUniformLocation, value1, value2, value3);
    }
    
    
    //eye pos
    void worldEyePositionUniformLocation() {
        _worldEyePositionUniformLocation = glGetUniformLocation(_programID, "u_worldEyePos");
    }
    
    void worldEyePositionUniform3f(GLfloat value1, GLfloat value2, GLfloat value3) {
        glUniform3f(_worldEyePositionUniformLocation, value1, value2, value3);
    }
    
    
    //for shadow
    void shadowViewProjectionMatUniformLocation() {
        _shadowViewProjectionMatLocation = glGetUniformLocation(_programID, "u_shadowViewProjectionMat");
    }

    void shadowViewProjectionMatUniformMatrix4fv(const GLfloat *value) {
        glUniformMatrix4fv(_shadowViewProjectionMatLocation, 1, GL_FALSE, value);
    }
    
    //for SSAO
    void ssaoTextureUniformLocation() {
        _ssaoTextureUniformLocation = glGetUniformLocation(_programID, "u_ssaoTexture");
    }
    
protected:
    
    GLint _texCoordAttribLocation;
    
    GLint _posTextureUniformLocation;
    GLint _normalTextureUniformLocation;
    GLint _albedoTextureUniformLocation;
    GLint _shadowDepthUniformLocation;
    
    GLint _worldLightPosUniformLocation;
    GLint _lightCountUniformLocation;
    
    GLint _diffuseColorUniformLocation;
    GLint _ambientColorUniformLocation;
    GLint _specularColorUniformLocation;
    GLint _worldEyePositionUniformLocation;
    
    GLint _shadowViewProjectionMatLocation;
    
    GLint _ssaoTextureUniformLocation;
    
};



#endif /* DeferredLightingShader_hpp */

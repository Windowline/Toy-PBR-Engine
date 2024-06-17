#ifndef Shader_hpp
#define Shader_hpp

#include "ShaderBase.hpp"

/**
 * 기본 퐁 셰이딩을 해주는 셰이더입니다.
 * 개발 초기에 타 기능과 비교를 위해서 만었고
 * 현재는 빛 관련 속성이 필요한 셰이더의 부모로서 쓰입니다.
 */
class BasicLightingShader : public ShaderBase {
    
   public:
    
    BasicLightingShader();
    virtual bool load() override;
    virtual void useProgram() override;
    
    //uniform
    void worldUniformLocation() {
        _worldMatUniformLocation = glGetUniformLocation(_programID, "u_worldMat");
    }
    
    void worldMatUniformMatrix4fv(const GLfloat *value) {
        glUniformMatrix4fv(_worldMatUniformLocation, 1, GL_FALSE, value);
    }
    
    
    //modelView Mat
    void worldViewUniformLocation() {
        _worldViewMatUniformLocation = glGetUniformLocation(_programID, "u_worldViewMat");
    }
    
    void worldViewMatUniformMatrix4fv(const GLfloat *value) {
        glUniformMatrix4fv(_worldViewMatUniformLocation, 1, GL_FALSE, value);
    }
    
    //proj mat
    void projUniformLocation() {
        _projMatUniformLocation = glGetUniformLocation(_programID, "u_projMat");
    }
    
    void projMatUniformMatrix4fv(const GLfloat *value) {
        glUniformMatrix4fv(_projMatUniformLocation, 1, GL_FALSE, value);
    }
    
    //normal mat
    void worldNormalMatUniformLocation() {
        _worldNormalMatUniformLocation = glGetUniformLocation(_programID, "u_worldNormalMat");
    }
    
    void worldNormalMatUniformMatrix4fv(const GLfloat *value) {
        glUniformMatrix4fv(_worldNormalMatUniformLocation, 1, GL_FALSE, value);
    }
    
    //light pos
    void worldLightPosUniformLocation() {
        _worldLightPosUniformLocation = glGetUniformLocation(_programID, "u_worldLightPos");
    }
    
    void worldLightPosUniform3fv(const GLfloat* v, GLsizei count) {
        glUniform3fv(_worldLightPosUniformLocation, count, v);
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
    
    
    
protected:
    //attrib
    GLint _colorAttribLocation;
    GLint _normalAttribLocation;
    
    //uniform
    GLint _worldMatUniformLocation;
    GLint _worldViewMatUniformLocation;
    GLint _projMatUniformLocation;
    
    GLint _worldLightPosUniformLocation;
    GLint _diffuseColorUniformLocation;
    GLint _ambientColorUniformLocation;
    GLint _specularColorUniformLocation;
    GLint _worldEyePositionUniformLocation;
    GLint _worldNormalMatUniformLocation;
};

#endif /* Shader_hpp */

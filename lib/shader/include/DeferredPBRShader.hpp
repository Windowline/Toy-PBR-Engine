#ifndef TOYRENDERER_DEFERREDPBRSHADER_HPP
#define TOYRENDERER_DEFERREDPBRSHADER_HPP

#include "BasicShader.hpp"
#include "Vector.hpp"

class DeferredPBRShader : public BasicShader {
public:
    DeferredPBRShader();
    bool load() override;
    void useProgram() override;

    void posTextureUniformLocation() {
        _posTextureUniformLocation = glGetUniformLocation(_programID, "u_posTexture");
    }

    void posTextureUniform1i(GLuint value) {
        assert(_posTextureUniformLocation != -1);
        glUniform1i(_posTextureUniformLocation, value);
    }


    void normalTextureUniformLocation() {
        _normalTextureUniformLocation = glGetUniformLocation(_programID, "u_normalTexture");
    }

    void normalTextureUniform1i(GLuint value) {
        assert(_normalTextureUniformLocation != -1);
        glUniform1i(_normalTextureUniformLocation, value);
    }


    void albedoTextureUniformLocation() {
        _albedoTextureUniformLocation = glGetUniformLocation(_programID, "u_albedoTexture");
    }

    void albedoTextureUniform1i(GLuint value) {
        assert(_albedoTextureUniformLocation != -1);
        glUniform1i(_albedoTextureUniformLocation, value);
    }

    void shadowDepthUniformLocation() {
        _shadowDepthUniformLocation = glGetUniformLocation(_programID, "u_shadowDepth");
    }

    void shadowDepthUniform1i(GLuint value) {
        assert(_shadowDepthUniformLocation != -1);
        glUniform1i(_shadowDepthUniformLocation, value);
    }


    //light pos
    void worldLightPosUniformLocation() {
        _worldLightPosUniformLocation = glGetUniformLocation(_programID, "u_worldLightPos");
    }

    void worldLightPosUniform3fv(const GLfloat* v, GLsizei count) {
        assert(_worldLightPosUniformLocation != -1);
        glUniform3fv(_worldLightPosUniformLocation, count, v);
    }

    //position or color
    void lightUniform3fVector(const std::vector<vec3>& lightPositions, bool isPosition=true) {
        const int LIGHT_COMPONENT_COUNT = 3;
        float* lightPosArray = new float[lightPositions.size() * LIGHT_COMPONENT_COUNT];
        int idx = 0;
        for (auto& lightPos : lightPositions) {
            lightPosArray[idx++] = lightPos.x;
            lightPosArray[idx++] = lightPos.y;
            lightPosArray[idx++] = lightPos.z;
        }

        if (isPosition) {
            worldLightPosUniform3fv(lightPosArray, (int)lightPositions.size());
        } else {
            lightColorsUniformVec3fv(lightPosArray, (int)lightPositions.size());
        }

        lightCountUniform1i((int)lightPositions.size());

        delete[] lightPosArray;
    }

    void lightCountUniformLocation() {
        _lightCountUniformLocation = glGetUniformLocation(_programID, "u_lightCount");
    }

    void lightCountUniform1i(GLint value) {
        assert(_lightCountUniformLocation != -1);
        glUniform1i(_lightCountUniformLocation, value);
    }


    //diffuse color
    void diffuseColorUniformLocation() {
        _diffuseColorUniformLocation = glGetUniformLocation(_programID, "u_diffuseColor");
    }

    void diffuseColorUniform3f(GLfloat value1, GLfloat value2, GLfloat value3) {
        assert(_diffuseColorUniformLocation != -1);
        glUniform3f(_diffuseColorUniformLocation, value1, value2, value3);
    }

    //specular color
    void specularColorUniformLocation() {
        _specularColorUniformLocation = glGetUniformLocation(_programID, "u_specularColor");
    }

    void specularColorUniform3f(GLfloat value1, GLfloat value2, GLfloat value3) {
        assert(_specularColorUniformLocation != -1);
        glUniform3f(_specularColorUniformLocation, value1, value2, value3);
    }


    //ambient
    void ambientColorUniformLocation() {
        _ambientColorUniformLocation = glGetUniformLocation(_programID, "u_ambientColor");
    }

    void ambientColorUniform3f(GLfloat value1, GLfloat value2, GLfloat value3) {
        assert(_ambientColorUniformLocation != -1);
        glUniform3f(_ambientColorUniformLocation, value1, value2, value3);
    }


    //eye pos
    void worldEyePositionUniformLocation() {
        _worldEyePositionUniformLocation = glGetUniformLocation(_programID, "u_worldEyePos");
    }

    void worldEyePositionUniform3f(GLfloat value1, GLfloat value2, GLfloat value3) {
        assert(_worldEyePositionUniformLocation != -1);
        glUniform3f(_worldEyePositionUniformLocation, value1, value2, value3);
    }


    //for shadow
    void shadowViewProjectionMatUniformLocation() {
        _shadowViewProjectionMatLocation = glGetUniformLocation(_programID, "u_shadowViewProjectionMat");
    }

    void shadowViewProjectionMatUniformMatrix4fv(const GLfloat *value) {
        assert(_shadowViewProjectionMatLocation != -1);
        glUniformMatrix4fv(_shadowViewProjectionMatLocation, 1, GL_FALSE, value);
    }

    //for SSAO
    void ssaoTextureUniformLocation() {
        _ssaoTextureUniformLocation = glGetUniformLocation(_programID, "u_ssaoTexture");
    }


    //for IBL
    void metallicUniform1f(GLfloat v) {
        assert(_metallicLoc != -1);
        glUniform1f(_metallicLoc, v);
    }

    void roughnessUniform1f(GLfloat v) {
        assert(_roughnessLoc != -1);
        glUniform1f(_roughnessLoc, v);
    }

    void lightColorsUniformVec3fv(const GLfloat* v, int count) {
        assert(_lightColorsLoc != -1);
        glUniform3fv(_lightColorsLoc, count, v);
    }



private:
    GLint _posTextureUniformLocation = -1;
    GLint _normalTextureUniformLocation = -1;
    GLint _albedoTextureUniformLocation = -1;
    GLint _shadowDepthUniformLocation = -1;

    GLint _worldLightPosUniformLocation = -1;
    GLint _lightCountUniformLocation = -1;

    GLint _diffuseColorUniformLocation = -1;
    GLint _ambientColorUniformLocation = -1;
    GLint _specularColorUniformLocation = -1;
    GLint _worldEyePositionUniformLocation = -1;

    GLint _shadowViewProjectionMatLocation = -1;

    GLint _ssaoTextureUniformLocation = -1;

    //PBR
    GLint _metallicLoc = -1;
    GLint _roughnessLoc = -1;
    GLint _lightColorsLoc = -1;

    GLint _irradianceMapLoc = -1;
    GLint _prefilterMapLoc = -1;
    GLint _brdfLUTLoc = -1;
};


#endif //TOYRENDERER_DEFERREDPBRSHADER_HPP

#ifndef TOYRENDERER_DEFERREDPBRSHADER_HPP
#define TOYRENDERER_DEFERREDPBRSHADER_HPP

#include "BasicShader.hpp"
#include "Vector.hpp"

class DeferredPBRShader : public BasicShader {
public:
    DeferredPBRShader();
    bool load() override;
    void useProgram() override;

    void posTextureUniform1i(GLuint value) {
        assert(_posTextureUniformLoc != -1);
        glUniform1i(_posTextureUniformLoc, value);
    }

    void normalTextureUniform1i(GLuint value) {
        assert(_normalTextureUniformLoc != -1);
        glUniform1i(_normalTextureUniformLoc, value);
    }


    void albedoTextureUniform1i(GLuint value) {
        assert(_albedoTextureUniformLoc != -1);
        glUniform1i(_albedoTextureUniformLoc, value);
    }

    void shadowDepthUniform1i(GLuint value) {
        assert(_shadowDepthUniformLoc != -1);
        glUniform1i(_shadowDepthUniformLoc, value);
    }


    void worldLightPosUniform3fv(const GLfloat* v, GLsizei count) {
        assert(_worldLightPosUniformLoc != -1);
        glUniform3fv(_worldLightPosUniformLoc, count, v);
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

    void lightCountUniform1i(GLint value) {
        assert(_lightCountUniformLoc != -1);
        glUniform1i(_lightCountUniformLoc, value);
    }

    void worldEyePositionUniform3f(GLfloat value1, GLfloat value2, GLfloat value3) {
        assert(_worldEyePositionUniformLoc != -1);
        glUniform3f(_worldEyePositionUniformLoc, value1, value2, value3);
    }

    void shadowViewProjectionMatUniformMatrix4fv(const GLfloat *value) {
        assert(_shadowViewProjectionMatLoc != -1);
        glUniformMatrix4fv(_shadowViewProjectionMatLoc, 1, GL_FALSE, value);
    }

    //for SSAO
    void ssaoTextureUniformLocation() {
        assert(_ssaoTextureUniformLocation != -1);
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
    GLint _posTextureUniformLoc = -1;
    GLint _normalTextureUniformLoc = -1;
    GLint _albedoTextureUniformLoc = -1;
    GLint _shadowDepthUniformLoc = -1;
    GLint _worldLightPosUniformLoc = -1;
    GLint _lightCountUniformLoc = -1;
    GLint _worldEyePositionUniformLoc = -1;
    GLint _shadowViewProjectionMatLoc = -1;
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

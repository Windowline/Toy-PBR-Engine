#ifndef ShadowDepthShader_hpp
#define ShadowDepthShader_hpp

#include "BasicLightingShader.hpp"

/**
 * 그림자를 위한 깊이 버퍼를 그리는 셰이더입니다.
 */
class ShadowDepthShader : public BasicLightingShader {

public:
    ShadowDepthShader();
    virtual bool load() override;
    virtual void useProgram() override;

    
    void shadowMVPUniformLocation() {
        _shadowMVPUniformLocation = glGetUniformLocation(_programID, "u_shadowMVP");
    }
    
    void shadowMVPUniformMatrix4fv(const GLfloat *value) {
        glUniformMatrix4fv(_shadowMVPUniformLocation, 1, GL_FALSE, value);
    }
    
    
protected:

    GLint _shadowMVPUniformLocation;
};


#endif

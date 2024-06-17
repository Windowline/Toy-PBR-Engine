
#ifndef AlbedoColor_hpp
#define AlbedoColor_hpp

#include "ShaderBase.hpp"

/**
 * 3d 오브젝트에 색상을 입히는 셰이더입니다.
 * 방 천장의 빛을 표현하는 구를 그립니다.
 */
class AlbedoColorShader : public ShaderBase {
public:
    AlbedoColorShader();
    virtual bool load() override;
    virtual void useProgram() override;

    void mvpUniformLocation() {
        _mvpUniformLocation =  glGetUniformLocation(_programID, "u_mvp");
    }
    
    void mvpUniformMatrix4fv(const GLfloat *value) {
        glUniformMatrix4fv(_mvpUniformLocation, 1, GL_FALSE, value);
    }
    
private:
    GLint _mvpUniformLocation;
    
};
#endif /* AlbedoColor_hpp */

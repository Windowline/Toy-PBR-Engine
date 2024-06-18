#ifndef TOYRENDERER_SHADOWDEPTHSHADERTMP_HPP
#define TOYRENDERER_SHADOWDEPTHSHADERTMP_HPP

#include "TestShaderTmp.hpp"

class ShadowDepthShaderTmp : public TestShaderTmp {
public:
    ShadowDepthShaderTmp();
    bool load() override;

    void shadowMVPUniformMatrix4fv(const GLfloat *value) const {
        glUniformMatrix4fv(_shadowMVPUniformLocation, 1, GL_FALSE, value);
    }

private:
    void shadowMVPUniformLocation() {
        _shadowMVPUniformLocation = glGetUniformLocation(_programID, "u_shadowMVP");
    }

    int _shadowMVPUniformLocation = 0;
};


#endif //TOYRENDERER_SHADOWDEPTHSHADERTMP_HPP

#ifndef TOYRENDERER_SHADOWDEPTHSHADERTMP_HPP
#define TOYRENDERER_SHADOWDEPTHSHADERTMP_HPP

#include "BasicShader.hpp"

class ShadowDepthShaderTmp : public BasicShader {
public:
    ShadowDepthShaderTmp();
    bool load() override;

    void shadowMVPUniformMatrix4fv(const GLfloat *value) const {
        assert(_shadowMVPUniformLoc != -1);
        glUniformMatrix4fv(_shadowMVPUniformLoc, 1, GL_FALSE, value);
    }

private:
    void shadowMVPUniformLocation() {
        _shadowMVPUniformLoc = glGetUniformLocation(_programID, "u_shadowMVP");
    }

    int _shadowMVPUniformLoc = -1;
};


#endif //TOYRENDERER_SHADOWDEPTHSHADERTMP_HPP

#ifndef TOYRENDERER_SHADOWDEPTHSHADER_HPP
#define TOYRENDERER_SHADOWDEPTHSHADER_HPP

#include "BasicShader.hpp"

class ShadowDepthShader : public BasicShader {
public:
    ShadowDepthShader();
    bool load() override;

    void shadowMVPUniformMatrix4fv(const GLfloat *value, int count) const {
        assert(_shadowMVPUniformLoc != -1);
        glUniformMatrix4fv(_shadowMVPUniformLoc, count, GL_FALSE, value);
    }

private:
    void shadowMVPUniformLocation() {
        _shadowMVPUniformLoc = glGetUniformLocation(_programID, "u_shadowMVP");
    }

    int _shadowMVPUniformLoc = -1;
};


#endif //TOYRENDERER_SHADOWDEPTHSHADER_HPP

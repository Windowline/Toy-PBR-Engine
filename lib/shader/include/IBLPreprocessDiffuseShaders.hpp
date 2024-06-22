#ifndef TOYRENDERER_EQUIRECTANGULARTOCUBEMAPSHADER_HPP
#define TOYRENDERER_EQUIRECTANGULARTOCUBEMAPSHADER_HPP

#include "BasicShader.hpp"

class EquirectangularToCubemapShader : public BasicShader {
public:
    EquirectangularToCubemapShader();
    bool load() override;
    void useProgram() override;

private:
    unsigned int _equirectangularMapLoc = -1;
};

class IrradianceShader : public BasicShader {
public:
    IrradianceShader();
    bool load() override;
    void useProgram() override;

private:
    unsigned int _environmentMapLoc = -1;
};

#endif //TOYRENDERER_EQUIRECTANGULARTOCUBEMAPSHADER_HPP

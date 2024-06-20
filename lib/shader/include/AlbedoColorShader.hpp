#ifndef TOYRENDERER_ALBEDOCOLORSHADER_HPP
#define TOYRENDERER_ALBEDOCOLORSHADER_HPP

#include "BasicShader.hpp"

class AlbedoColorShader : public BasicShader {
public:
    AlbedoColorShader();
    virtual bool load() override;
};

#endif //TOYRENDERER_ALBEDOCOLORSHADER_HPP

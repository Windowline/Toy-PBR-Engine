#ifndef TOYRENDERER_ALBEDOCOLORSHADERTMP_HPP
#define TOYRENDERER_ALBEDOCOLORSHADERTMP_HPP

#include "BasicShader.hpp"

class AlbedoColorShaderTmp : public BasicShader {
public:
    AlbedoColorShaderTmp();
    virtual bool load() override;
};

#endif //TOYRENDERER_ALBEDOCOLORSHADERTMP_HPP

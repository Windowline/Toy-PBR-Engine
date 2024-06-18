#ifndef TOYRENDERER_TESTSHADER_H
#define TOYRENDERER_TESTSHADER_H

#include "ShaderBase.hpp"

class TestShader : public ShaderBase {
public:
    TestShader();
    virtual bool load() override;
    virtual void useProgram() override;
};

#endif //TOYRENDERER_TESTSHADER_H

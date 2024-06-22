#ifndef TOYRENDERER_BGSHADER_HPP
#define TOYRENDERER_BGSHADER_HPP

#include "BasicShader.hpp"

class BGShader : public BasicShader {
public:
    BGShader();
    bool load() override;
    void useProgram() override;

private:
    unsigned int _equirectangularMapLoc = -1;

};

#endif //TOYRENDERER_BGSHADER_HPP

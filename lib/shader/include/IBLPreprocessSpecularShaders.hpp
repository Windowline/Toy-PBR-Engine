#ifndef TOYRENDERER_IBLPREPROCESSSPECULARSHADERS_HPP
#define TOYRENDERER_IBLPREPROCESSSPECULARSHADERS_HPP

#include "BasicShader.hpp"

class PrefilterShader : public BasicShader {
public:
    PrefilterShader();
    bool load() override;
    void useProgram() override;

    void roughnesUniformLocation1f(GLfloat value) {
        glUniform1f(_roughnessLoc, value);
    }

private:
    unsigned int _environmentMapLoc = -1;
    unsigned int _roughnessLoc = -1;
};


class BRDFShader : public BasicShader {
public:
    BRDFShader();
    bool load() override;
    void useProgram() override;

private:

};


#endif //TOYRENDERER_IBLPREPROCESSSPECULARSHADERS_HPP

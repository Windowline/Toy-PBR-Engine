#ifndef TOYRENDERER_SIMPLESHADER_HPP
#define TOYRENDERER_SIMPLESHADER_HPP

#include "BasicShader.hpp"

class SimpleShader : public BasicShader {
public:
    SimpleShader();
    bool load() override;
    void useProgram() override;
    void setColorUnifrom3f(GLfloat v1, GLfloat v2, GLfloat v3) {
        assert(_colorLoc != -1);
        glUniform3f(_colorLoc, v1, v2, v3);
    }

private:
    unsigned int _colorLoc = -1;

};

#endif //TOYRENDERER_SIMPLESHADER_HPP

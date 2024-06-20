
#ifndef TOYRENDERER_CUBE_HPP
#define TOYRENDERER_CUBE_HPP
#include "BasicMeshInterface.h"
#include "Vector.hpp"

class Cube : public BasicMeshInterface {
public:
    Cube(int size, vec3 color);
    void render() const override;

private:
    unsigned int _VAO;
    unsigned int _VBO;
    unsigned int _EBO;

    int _size;
    vec3 _color;
    int _indSize;
};


#endif //TOYRENDERER_CUBE_HPP

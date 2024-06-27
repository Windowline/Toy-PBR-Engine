
#ifndef TOYRENDERER_CUBE_HPP
#define TOYRENDERER_CUBE_HPP
#include "MeshBasic.h"
#include "Vector.hpp"

class Cube : public MeshBasic {
public:
    Cube(int size, vec3 color, std::string name);

private:
    int _size;
    vec3 _color;
};


#endif //TOYRENDERER_CUBE_HPP

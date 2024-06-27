#ifndef TOYRENDERER_PLANE_HPP
#define TOYRENDERER_PLANE_HPP

#include "MeshBasic.h"
#include "Vector.hpp"

class Plane : public MeshBasic {
public:
    Plane(float size, vec3 color, std::string name);

private:
    float _size;
    vec3 _color;
};




#endif //TOYRENDERER_PLANE_HPP

#ifndef TOYRENDERER_SPHERE_HPP
#define TOYRENDERER_SPHERE_HPP

#include "MeshBasic.h"
#include "Vector.hpp"

class Sphere : public MeshBasic {
public:
    Sphere(float radius, vec3 color, std::string name);
    void render(int instanceCount=1) const override;

private:
    float _radius;
    vec3 _color;
};


#endif //TOYRENDERER_SPHERE_HPP

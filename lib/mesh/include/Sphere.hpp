#ifndef TOYRENDERER_SPHERE_HPP
#define TOYRENDERER_SPHERE_HPP

#include "BasicMeshInterface.h"
#include "Vector.hpp"

class Sphere : public BasicMeshInterface {
public:
    Sphere(float radius, vec3 color);
    void render() const override;

private:
    unsigned int _VAO;
    unsigned int _VBO;
    unsigned int _EBO;

    float _radius;
    vec3 _color;
    unsigned int _indSize;

};


#endif //TOYRENDERER_SPHERE_HPP

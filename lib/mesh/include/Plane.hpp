#ifndef TOYRENDERER_PLANE_HPP
#define TOYRENDERER_PLANE_HPP

#include "BasicMeshInterface.h"
#include "Vector.hpp"

class Plane : public BasicMeshInterface {
public:
    Plane(float size, vec3 color);
    void render() const override;

private:
    unsigned int _VAO = -1;
    unsigned int _VBO = -1;
    unsigned int _EBO = -1;
    float _size;
    int _indSize;
    vec3 _color;
};




#endif //TOYRENDERER_PLANE_HPP

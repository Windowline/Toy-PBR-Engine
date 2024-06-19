#ifndef TOYRENDERER_SPHERE2_HPP
#define TOYRENDERER_SPHERE2_HPP

#include "Vector.hpp"

class Sphere2 {
public:
    Sphere2(float radius, vec3 color);
    void render() const;

private:
    unsigned int _VAO;
    unsigned int _VBO;
    unsigned int _EBO;

    float _radius;
    vec3 _color;
    int _indSize;

};


#endif //TOYRENDERER_SPHERE2_HPP

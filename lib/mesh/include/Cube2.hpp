
#ifndef TOYRENDERER_CUBE2_HPP
#define TOYRENDERER_CUBE2_HPP

#include "Vector.hpp"

class Cube2 {
public:
    Cube2(int size, vec3 color);
    void render() const;
private:
    unsigned int _VAO;
    unsigned int _VBO;
    unsigned int _EBO;

    int _size;
    vec3 _color;
    int _indSize;
};


#endif //TOYRENDERER_CUBE2_HPP

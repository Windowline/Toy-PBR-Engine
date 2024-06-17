

#ifndef Cube_hpp
#define Cube_hpp

#include "Mesh.hpp"

class Cube : public Mesh {
public:
    Cube(float size, vec3 color);
    
protected:
    void buildMesh() override;
    
private:
    float _size;
    vec3 _color;
};


#endif /* Cube_hpp */

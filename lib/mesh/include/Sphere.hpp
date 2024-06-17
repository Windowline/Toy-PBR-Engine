

#ifndef Sphere_hpp
#define Sphere_hpp

#include "Mesh.hpp"


class Sphere : public Mesh {
public:
    Sphere(float radius, vec3 color);
    
protected:
    void buildMesh() override;
    //void buildMeshTest();
private:
    float _radius;
    vec3 _color;
};



#endif /* Sphere_hpp */

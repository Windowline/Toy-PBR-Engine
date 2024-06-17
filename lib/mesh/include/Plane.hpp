

#ifndef Plane_hpp
#define Plane_hpp

#include "Mesh.hpp"


class Plane : public Mesh {
public:
    Plane(vec2 size);
    
protected:
    void buildMesh() override;
    
private:
    vec2 _size;
    
};



#endif /* Plane_hpp */

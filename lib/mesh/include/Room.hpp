
#ifndef Room_hpp
#define Room_hpp

#include "Mesh.hpp"

class Room : public Mesh {
public:
    Room(float size, vec3 backColor, vec3 topColor,
        vec3 leftColor, vec3 rightColor, vec3 bottomColor);
    
protected:
    void buildMesh() override;
    
private:
    float _size;
    vec3 _topColor;
    vec3 _backColor;
    vec3 _leftColor;
    vec3 _rightColor;
    vec3 _bottomColor;
};


#endif /* Room_hpp */

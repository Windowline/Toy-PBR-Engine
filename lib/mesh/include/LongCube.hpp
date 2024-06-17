//
//  LongCube.hpp
//  RenderTest
//
//  Created by 박창선 on 2022/12/31.
//

#ifndef LongCube_hpp
#define LongCube_hpp

#include "Mesh.hpp"

class LongCube : public Mesh {
public:
    LongCube(float size, vec3 color);

protected:
    void buildMesh() override;
    
private:
    float _size;
    vec3 _color;
    
};


#endif /* LongCube_hpp */

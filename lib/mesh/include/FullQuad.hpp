#ifndef TOYRENDERER_FULLQUAD_H
#define TOYRENDERER_FULLQUAD_H
#include "MeshBasic.h"

class FullQuad : public MeshBasic {
public:
    FullQuad(std::string name);
    void render(int instanceCount = 1) const override;

};


#endif //TOYRENDERER_FULLQUAD_H

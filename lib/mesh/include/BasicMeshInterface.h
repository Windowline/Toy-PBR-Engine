#ifndef TOYRENDERER_BASICMESHINTERFACE_H
#define TOYRENDERER_BASICMESHINTERFACE_H

class BasicMeshInterface {
public:
    virtual void render() const = 0;
//    virtual ~BasicMeshInterface() = default;
};

#endif //TOYRENDERER_BASICMESHINTERFACE_H

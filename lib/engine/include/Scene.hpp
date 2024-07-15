#ifndef TOYRENDERER_SCENE_HPP
#define TOYRENDERER_SCENE_HPP

#include <memory>

class Scene {
public:
    virtual void setScreenSize(int w, int h) = 0;
    virtual void updateViewPosition(int dir, float delta) = 0;
    virtual void updateViewRotation(float yaw, float pitch) = 0;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual ~Scene() {}
};

#endif //TOYRENDERER_SCENE_HPP

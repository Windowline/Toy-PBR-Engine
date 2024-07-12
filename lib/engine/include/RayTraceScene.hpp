#ifndef TOYRENDERER_RAYTRACESCENE_HPP
#define TOYRENDERER_RAYTRACESCENE_HPP

#include <functional>
#include <memory>
#include "Vector.hpp"
#include "Matrix.hpp"

class RenderEngine;
class Camera;
class ShaderManager;
class FrameBufferObject;
class FullQuad;

class RayTraceScene {
public:
    RayTraceScene(RenderEngine* engine, unsigned int defaultFBO);

    virtual ~RayTraceScene();

    void setScreenSize(int w, int h);

    void updateViewPosition(int dir, float delta);

    void updateViewRotation(float yaw, float pitch);

    void update();

    void render();

    std::shared_ptr<ShaderManager> shaderManager();
//
//    std::shared_ptr<Camera> camera() {
//        return _camera;
//    }

private:
    RenderEngine* _engine;
    unsigned int _defaultFBO;
    std::shared_ptr<Camera> _camera;

    std::unique_ptr<FullQuad> _fullQuad;
};


#endif //TOYRENDERER_RAYTRACESCENE_HPP

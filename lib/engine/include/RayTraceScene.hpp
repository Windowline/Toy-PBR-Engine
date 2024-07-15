#ifndef TOYRENDERER_RAYTRACESCENE_HPP
#define TOYRENDERER_RAYTRACESCENE_HPP

#include <functional>
#include <memory>
#include "Vector.hpp"
#include "Matrix.hpp"
#include "Scene.hpp"

class RenderEngine;
class Camera;
class ShaderManager;
class FrameBufferObject;
class FullQuad;
class Node;
class Model;
class BVHRayTraceShader;
struct BVHNode;

class RayTraceScene : public Scene {
public:
    RayTraceScene(RenderEngine* engine, unsigned int defaultFBO);

    virtual ~RayTraceScene();

    void setScreenSize(int w, int h) override;

    void updateViewPosition(int dir, float delta) override;

    void updateViewRotation(float yaw, float pitch) override;

    void update() override;

    void render() override;

    std::shared_ptr<ShaderManager> shaderManager();

private:
    void buildMeshTBO(bool bvh);
    void buildTestTri();

    bool _applyBVH = true;

    RenderEngine* _engine;
    unsigned int _defaultFBO;
    std::shared_ptr<Camera> _camera;
    std::unique_ptr<FullQuad> _fullQuad;
    std::shared_ptr<Node> _rootNode;
    std::shared_ptr<Node> _modelNode;
    std::shared_ptr<Model> _modelMesh;

    std::shared_ptr<BVHRayTraceShader> _bvhRayTraceShader;

    //no BVH
    unsigned int _posTBO = 0;
    unsigned int _posTBOTexture = 0;
    unsigned int _normalTBO = 0;
    unsigned int _normalTBOTexture = 0;
    unsigned int _modelTriangleSize = 0;

    // BVH index
    unsigned int _bvhNodeTBO = 0; // [triIndex, triCount, childIndex]
    unsigned int _bvhNodeTBOTexture = 0;

    // BVH aabb-min
    unsigned int _bvhMinBoundsTBO = 0;
    unsigned int _bvhMinBoundsTBOTexture = 0;

    // BVH aabb-max
    unsigned int _bvhMaxBoundsTBO = 0;
    unsigned int _bvhMaxBoundsTBOTexture = 0;

    // BVH triangle
    unsigned int _bvhTriangleTBO = 0;
    unsigned int _bvhTriangleTBOTexture = 0;

    vec3 _boundsMin = vec3(-1e9, -1e9, -1e9);
    vec3 _boundsMax = vec3(1e9, 1e9, 1e9);
};


#endif //TOYRENDERER_RAYTRACESCENE_HPP

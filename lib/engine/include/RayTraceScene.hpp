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
class Model;
class BVHRayTraceShader;
struct BVHNode;
struct Triangle;

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
    void buildMeshTBO();

    void renderBVHForDebug();

    RenderEngine* _engine;
    unsigned int _defaultFBO;

    std::shared_ptr<Camera> _camera;
    std::unique_ptr<FullQuad> _fullQuad;

    std::shared_ptr<Model> _modelMesh;

    std::shared_ptr<BVHRayTraceShader> _bvhRayTraceShader;

    //BVH Triangle Pos/N
    unsigned int _posTBO = 0;
    unsigned int _posTBOTexture = 0;
    unsigned int _normalTBO = 0;
    unsigned int _normalTBOTexture = 0;

    // BVH index
    unsigned int _bvhNodeTBO = 0; // [triIndex, triCount, childIndex]
    unsigned int _bvhNodeTBOTexture = 0;

    // BVH aabb
    unsigned int _bvhMinBoundsTBO = 0;
    unsigned int _bvhMinBoundsTBOTexture = 0;
    unsigned int _bvhMaxBoundsTBO = 0;
    unsigned int _bvhMaxBoundsTBOTexture = 0;

    // Room Triangle Pos/N
    unsigned int _roomPosTBO = 0;
    unsigned int _roomPosTBOTexture = 0;
    unsigned int _roomNormalTBO = 0;
    unsigned int _roomNormalTBOTexture = 0;

    const int BVH_MAX_DEPTH = 10 + 1;
    std::vector<BVHNode> _bvhNodes;
    std::vector<Triangle> _bvhTriangles;
    int _bvhLeafStartIdx;
    int _bvhLeafLastIdx;

    bool _debugBVH = false;
};


#endif //TOYRENDERER_RAYTRACESCENE_HPP

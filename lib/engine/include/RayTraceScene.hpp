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
class Node;
class Model;
struct BVHNode;

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

private:
    void buildMeshTBO();
    void buildTestTri();

    RenderEngine* _engine;
    unsigned int _defaultFBO;
    std::shared_ptr<Camera> _camera;

    std::unique_ptr<FullQuad> _fullQuad;

    std::shared_ptr<Node> _rootNode;
    std::shared_ptr<Node> _modelNode;
    std::shared_ptr<Model> _modelMesh;

    unsigned int _posTBO = 0;
    unsigned int _posTBOTexture = 0;
    unsigned int _normalTBO = 0;
    unsigned int _normalTBOTexture = 0;
    unsigned int _modelTriangleSize = 0;

    vec3 _boundsMin = vec3(-1e9, -1e9, -1e9);
    vec3 _boundsMax = vec3(1e9, 1e9, 1e9);

    std::vector<std::shared_ptr<BVHNode>> _bvhRoots;
};


#endif //TOYRENDERER_RAYTRACESCENE_HPP

#include "RayTraceScene.hpp"
#include "Engine.hpp"
#include "Camera.hpp"
#include "GLUtilGeometry.hpp"
#include "FullQuad.hpp"

#include "ShaderManager.hpp"
#include "BVHRayTraceShader.hpp"
#include "PathInfo.h"

#include "MeshBasic.h"
#include "Model.hpp"
#include "Node.hpp"
#include "BVH.hpp"

using namespace std;

RayTraceScene::RayTraceScene(RenderEngine* engine, GLuint defaultFBO) {
    _engine = engine;
    _defaultFBO = defaultFBO;

    _fullQuad = make_unique<FullQuad>("FullQuad");
    _camera = make_shared<Camera>(vec3(0, 0, 20), vec3(0, 0, 0));

    _rootNode = make_shared<Node>(nullptr, make_shared<MeshBasic>(), mat4());
    _rootNode->setEnabled(false);

    _modelMesh = make_shared<Model>(RESOURCE_DIR + "/objects/cyborg/cyborg.obj", vec3(0.75, 0.75, 0.75), "Model");
    _modelNode = make_shared<Node>(nullptr, _modelMesh, mat4());
    _rootNode->addChild(_modelNode);

    buildMeshTBO();

    //setup shader
    _bvhRayTraceShader = shaderManager()->setActiveShader<BVHRayTraceShader>(eShaderProgram_BVHRayTrace);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_BUFFER, _bvhNodeTBOTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, _bvhMinBoundsTBOTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_BUFFER, _bvhMaxBoundsTBOTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_BUFFER, _posTBOTexture);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_BUFFER, _normalTBOTexture);
}

void RayTraceScene::buildMeshTBO() {
    vector<BVHNode> bvhNodes;
    vector<Triangle> bvhTriangles;

    for (const ModelMesh& mesh : _modelMesh->meshes) {
        buildBVH(mesh.vertices, mesh.indices, BVH_MAX_DEPTH, bvhNodes, bvhTriangles);
        break; // TODO: multi mesh
    }

    vector<float> bvhNodeIndices; // [triIndex, triCount, childIndex]
    vector<float> bvhMinBounds;
    vector<float> bvhMaxBounds;
    vector<float> trianglePos;
    vector<float> triangleNormal;

    bvhNodeIndices.reserve(bvhNodes.size() * 3);
    bvhMinBounds.reserve(bvhNodes.size() * 3);
    bvhMaxBounds.reserve(bvhNodes.size() * 3);
    trianglePos.reserve(bvhTriangles.size() * 9);
    triangleNormal.reserve(bvhTriangles.size() * 9);

    // Node Info
    for (const auto& node : bvhNodes) {
        bvhNodeIndices.push_back(node.triangleStartIdx);
        bvhNodeIndices.push_back(node.triangleCnt);
        bvhNodeIndices.push_back(node.nodeIdx);

        bvhMinBounds.push_back(node.aabb.boundsMin.x);
        bvhMinBounds.push_back(node.aabb.boundsMin.y);
        bvhMinBounds.push_back(node.aabb.boundsMin.z);

        bvhMaxBounds.push_back(node.aabb.boundsMax.x);
        bvhMaxBounds.push_back(node.aabb.boundsMax.y);
        bvhMaxBounds.push_back(node.aabb.boundsMax.z);
    }

    // Triangle Info
    for (const auto& tri : bvhTriangles) {
        trianglePos.push_back(tri.posA.x);
        trianglePos.push_back(tri.posA.y);
        trianglePos.push_back(tri.posA.z);

        trianglePos.push_back(tri.posB.x);
        trianglePos.push_back(tri.posB.y);
        trianglePos.push_back(tri.posB.z);

        trianglePos.push_back(tri.posC.x);
        trianglePos.push_back(tri.posC.y);
        trianglePos.push_back(tri.posC.z);

        triangleNormal.push_back(tri.NA.x);
        triangleNormal.push_back(tri.NA.y);
        triangleNormal.push_back(tri.NA.z);

        triangleNormal.push_back(tri.NB.x);
        triangleNormal.push_back(tri.NB.y);
        triangleNormal.push_back(tri.NB.z);

        triangleNormal.push_back(tri.NC.x);
        triangleNormal.push_back(tri.NC.y);
        triangleNormal.push_back(tri.NC.z);
    }

    glGenBuffers(1, &_bvhNodeTBO);
    glBindBuffer(GL_TEXTURE_BUFFER, _bvhNodeTBO);
    glBufferData(GL_TEXTURE_BUFFER, bvhNodeIndices.size() * sizeof(float), bvhNodeIndices.data(), GL_STATIC_DRAW);
    glGenTextures(1, &_bvhNodeTBOTexture);
    glBindTexture(GL_TEXTURE_BUFFER, _bvhNodeTBOTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, _bvhNodeTBO); //GL_RGB16I : 안돼..

    glGenBuffers(1, &_bvhMinBoundsTBO);
    glBindBuffer(GL_TEXTURE_BUFFER, _bvhMinBoundsTBO);
    glBufferData(GL_TEXTURE_BUFFER, bvhMinBounds.size() * sizeof(float), bvhMinBounds.data(), GL_STATIC_DRAW);
    glGenTextures(1, &_bvhMinBoundsTBOTexture);
    glBindTexture(GL_TEXTURE_BUFFER, _bvhMinBoundsTBOTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, _bvhMinBoundsTBO);

    glGenBuffers(1, &_bvhMaxBoundsTBO);
    glBindBuffer(GL_TEXTURE_BUFFER, _bvhMaxBoundsTBO);
    glBufferData(GL_TEXTURE_BUFFER, bvhMaxBounds.size() * sizeof(float), bvhMaxBounds.data(), GL_STATIC_DRAW);
    glGenTextures(1, &_bvhMaxBoundsTBOTexture);
    glBindTexture(GL_TEXTURE_BUFFER, _bvhMaxBoundsTBOTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, _bvhMaxBoundsTBO);

    glGenBuffers(1, &_posTBO);
    glBindBuffer(GL_TEXTURE_BUFFER, _posTBO);
    glBufferData(GL_TEXTURE_BUFFER, trianglePos.size() * sizeof(float), trianglePos.data(), GL_STATIC_DRAW);

    glGenTextures(1, &_posTBOTexture);
    glBindTexture(GL_TEXTURE_BUFFER, _posTBOTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, _posTBO);

    glGenBuffers(1, &_normalTBO);
    glBindBuffer(GL_TEXTURE_BUFFER, _normalTBO);
    glBufferData(GL_TEXTURE_BUFFER, triangleNormal.size() * sizeof(float), triangleNormal.data(), GL_STATIC_DRAW);

    glGenTextures(1, &_normalTBOTexture);
    glBindTexture(GL_TEXTURE_BUFFER, _normalTBOTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, _normalTBO);

    _modelTriangleSize = trianglePos.size() / (3 * 3);
}

RayTraceScene::~RayTraceScene() {
}


void RayTraceScene::setScreenSize(int w, int h) {
    if (!_camera) {
        return;
    }

    glViewport(0, 0, w, h);
    _camera->setScreenRect(Rect{0, 0, w, h});
}

void RayTraceScene::updateViewPosition(int dir, float delta) {
    if (_camera) {
        _camera->updateViewPosition(dir, delta);
    }
}

void RayTraceScene::updateViewRotation(float yaw, float pitch) {
    if (_camera) {
        _camera->updateViewRotation(yaw, pitch);
    }
}

void RayTraceScene::update() {}

void RayTraceScene::render() {
    glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBO);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const mat4& proj = _camera->projMat();
    const mat4& view = _camera->viewMat();

    _bvhRayTraceShader->cameraPosUniform3f(_camera->eye().x, _camera->eye().y, _camera->eye().z);
    _bvhRayTraceShader->resolutionUniform2f((float)_camera->screenSize().x, (float)_camera->screenSize().y);
    _bvhRayTraceShader->viewMatUniformMatrix4fv(view.pointer());
    _bvhRayTraceShader->projMatUniformMatrix4fv(proj.pointer());
    _bvhRayTraceShader->triangleSizeUniform1i(_modelTriangleSize);
    _bvhRayTraceShader->bvhLeafStartIdxUniform1i(1 << (BVH_MAX_DEPTH - 1));

    _fullQuad->render();
}


shared_ptr<ShaderManager> RayTraceScene::shaderManager() {
    return _engine->_shaderManager;
}
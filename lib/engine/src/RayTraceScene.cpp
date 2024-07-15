#include "RayTraceScene.hpp"
#include "Engine.hpp"
#include "Camera.hpp"
#include "GLUtilGeometry.hpp"
#include "FullQuad.hpp"

#include "ShaderManager.hpp"
#include "RayTraceShader.hpp"
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

    buildMeshTBO(_applyBVH);


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

void RayTraceScene::buildMeshTBO(bool bvh=false) {
    vector<float> posBufferData;
    vector<float> normalBufferData;
    vector<BVHNode> bvhNodes;
    vector<Triangle> bvhTriangles;

    for (const ModelMesh& mesh : _modelMesh->meshes) {
        for (const Vertex& vertex : mesh.vertices) {
            posBufferData.push_back(vertex.Position.x);
            posBufferData.push_back(vertex.Position.y);
            posBufferData.push_back(vertex.Position.z);

            normalBufferData.push_back(vertex.Normal.x);
            normalBufferData.push_back(vertex.Normal.y);
            normalBufferData.push_back(vertex.Normal.z);

            _boundsMin.x = min(vertex.Position.x, _boundsMin.x);
            _boundsMin.y = min(vertex.Position.y, _boundsMin.y);
            _boundsMin.z = min(vertex.Position.z, _boundsMin.z);

            _boundsMax.x = max(vertex.Position.x, _boundsMax.x);
            _boundsMax.y = max(vertex.Position.y, _boundsMax.y);
            _boundsMax.z = max(vertex.Position.z, _boundsMax.z);
        }

        buildBVH(mesh.vertices, mesh.indices, BVH_MAX_DEPTH, bvhNodes, bvhTriangles);
        break; //handle only one mesh
    }

    if (bvh) {
        vector<float> bvhNodeIndices; // [triIndex, triCount, childIndex]
        vector<float> bvhMinBounds;
        vector<float> bvhMaxBounds;

        bvhNodeIndices.reserve(bvhNodes.size() * 3);
        bvhMinBounds.reserve(bvhNodes.size() * 3);
        bvhMaxBounds.reserve(bvhNodes.size() * 3);

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

        //==== tri test
        posBufferData.clear();
        normalBufferData.clear();
        for (const auto& tri : bvhTriangles) {
            posBufferData.push_back(tri.posA.x);
            posBufferData.push_back(tri.posA.y);
            posBufferData.push_back(tri.posA.z);

            posBufferData.push_back(tri.posB.x);
            posBufferData.push_back(tri.posB.y);
            posBufferData.push_back(tri.posB.z);

            posBufferData.push_back(tri.posC.x);
            posBufferData.push_back(tri.posC.y);
            posBufferData.push_back(tri.posC.z);


            normalBufferData.push_back(tri.NA.x);
            normalBufferData.push_back(tri.NA.y);
            normalBufferData.push_back(tri.NA.z);

            normalBufferData.push_back(tri.NB.x);
            normalBufferData.push_back(tri.NB.y);
            normalBufferData.push_back(tri.NB.z);

            normalBufferData.push_back(tri.NC.x);
            normalBufferData.push_back(tri.NC.y);
            normalBufferData.push_back(tri.NC.z);
        }

        //====

        cout << "bvhNodes.size(): " << bvhNodes.size() << endl;
        cout << "bvhNodeIndices.size(): " << bvhNodeIndices.size()  << endl;
        cout << "bvhMinBounds.size(): " << bvhNodeIndices.size()  << endl;
        cout << "bvhMaxBounds.sizee(): " << bvhMaxBounds.size() << endl;

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
        glBufferData(GL_TEXTURE_BUFFER, posBufferData.size() * sizeof(float), posBufferData.data(), GL_STATIC_DRAW);

        glGenTextures(1, &_posTBOTexture);
        glBindTexture(GL_TEXTURE_BUFFER, _posTBOTexture);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, _posTBO);

        glGenBuffers(1, &_normalTBO);
        glBindBuffer(GL_TEXTURE_BUFFER, _normalTBO);
        glBufferData(GL_TEXTURE_BUFFER, normalBufferData.size() * sizeof(float), normalBufferData.data(), GL_STATIC_DRAW);

        glGenTextures(1, &_normalTBOTexture);
        glBindTexture(GL_TEXTURE_BUFFER, _normalTBOTexture);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, _normalTBO);

        _modelTriangleSize = posBufferData.size() / (3 * 3);

    } else {
        glGenBuffers(1, &_posTBO);
        glBindBuffer(GL_TEXTURE_BUFFER, _posTBO);
        glBufferData(GL_TEXTURE_BUFFER, posBufferData.size() * sizeof(float), posBufferData.data(), GL_STATIC_DRAW);

        glGenTextures(1, &_posTBOTexture);
        glBindTexture(GL_TEXTURE_BUFFER, _posTBOTexture);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, _posTBO);

        glGenBuffers(1, &_normalTBO);
        glBindBuffer(GL_TEXTURE_BUFFER, _normalTBO);
        glBufferData(GL_TEXTURE_BUFFER, normalBufferData.size() * sizeof(float), normalBufferData.data(), GL_STATIC_DRAW);

        glGenTextures(1, &_normalTBOTexture);
        glBindTexture(GL_TEXTURE_BUFFER, _normalTBOTexture);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, _normalTBO);

        _modelTriangleSize = posBufferData.size() / (3 * 3);
    }
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

//    vec2 vpSize = _camera->viewportSize();
//    glViewport(0, 0, vpSize.x, vpSize.y);
    const mat4& proj = _camera->projMat();
    const mat4& view = _camera->viewMat();

    if (_applyBVH) {
        _bvhRayTraceShader->cameraPosUniform3f(_camera->eye().x, _camera->eye().y, _camera->eye().z);
        _bvhRayTraceShader->resolutionUniform2f((float)_camera->screenSize().x, (float)_camera->screenSize().y);
        _bvhRayTraceShader->viewMatUniformMatrix4fv(view.pointer());
        _bvhRayTraceShader->projMatUniformMatrix4fv(proj.pointer());
        _bvhRayTraceShader->triangleSizeUniform1i(_modelTriangleSize);
        _bvhRayTraceShader->bvhLeafStartIdxUniform1i( (1 << (BVH_MAX_DEPTH - 1)) );
        _fullQuad->render();

        cout << "BD: " << (1 << (BVH_MAX_DEPTH - 1)) << endl;

    } else {

        mat4 camLocal;
        auto shader = shaderManager()->setActiveShader<RayTraceShader>(eShaderProgram_RayTrace);
        shader->cameraPosUniform3f(_camera->eye().x, _camera->eye().y, _camera->eye().z);
        shader->cameraLocalToWorldMatUniformMatrix4fv(camLocal.pointer());
        shader->resolutionUniform2f((float)_camera->screenSize().x, (float)_camera->screenSize().y);
        shader->triangleSizeUniform1i(_modelTriangleSize);
        shader->boundsMinUniform3f(_boundsMin.x, _boundsMin.y, _boundsMin.z);
        shader->boundsMaxUniform3f(_boundsMax.x, _boundsMax.y, _boundsMax.z);
        shader->viewMatUniformMatrix4fv(view.pointer());
        shader->projMatUniformMatrix4fv(proj.pointer());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_BUFFER, _posTBOTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_BUFFER, _normalTBOTexture);

        _fullQuad->render();
    }
}


shared_ptr<ShaderManager> RayTraceScene::shaderManager() {
    return _engine->_shaderManager;
}


//for debug
void RayTraceScene::buildTestTri() {
    //TBO
    //must be ccw
    vector<float> posBufferData = {
            0.0,  25.f, -40.f,
            -25.0,  0.f,  -40.f,
            25.0,  0.f,  -40.f,

            0.f,    0.0f,  -20.0f,
            -5.f,   -10.f,  -20.0f,
            5.f,   -10.f,  -20.0f,
    };

    vector<float> normalBufferData = {
            0.0,  25.f, -40.f,
            25.0,  0.f,  -40.f,
            -25.0,  0.f, -40.f,

            0.f,    0.0f,  -20.0f,
            5.f,   -10.f,  -20.0f,
            -5.f,   -10.f,  -20.0f
    };


    glGenBuffers(1, &_posTBO);
    glBindBuffer(GL_TEXTURE_BUFFER, _posTBO);
    glBufferData(GL_TEXTURE_BUFFER, posBufferData.size() * sizeof(float), posBufferData.data(), GL_STATIC_DRAW);

    glGenTextures(1, &_posTBOTexture);
    glBindTexture(GL_TEXTURE_BUFFER, _posTBOTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, _posTBO);

    glGenBuffers(1, &_normalTBO);
    glBindBuffer(GL_TEXTURE_BUFFER, _normalTBO);
    glBufferData(GL_TEXTURE_BUFFER, normalBufferData.size() * sizeof(float), normalBufferData.data(), GL_STATIC_DRAW);

    glGenTextures(1, &_normalTBOTexture);
    glBindTexture(GL_TEXTURE_BUFFER, _normalTBOTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, _normalTBO);

    _modelTriangleSize = posBufferData.size() / (3 * 3);

    //build boundig box
    for (int i = 0; i < posBufferData.size(); i += 3) {
        vec3 p = vec3(posBufferData[i + 0], posBufferData[i + 1], posBufferData[i + 2]);

        _boundsMin.x = min(p.x, _boundsMin.x);
        _boundsMin.y = min(p.y, _boundsMin.y);
        _boundsMin.z = min(p.z, _boundsMin.z);

        _boundsMax.x = max(p.x, _boundsMax.x);
        _boundsMax.y = max(p.y, _boundsMax.y);
        _boundsMax.z = max(p.z, _boundsMax.z);
    }
}
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
#include "BVH.hpp"
#include "SimpleShader.hpp"

using namespace std;

RayTraceScene::RayTraceScene(RenderEngine* engine, GLuint defaultFBO) {
    _engine = engine;
    _defaultFBO = defaultFBO;

    _fullQuad = make_unique<FullQuad>("FullQuad");
    _camera = make_shared<Camera>(vec3(0, 0, 15), vec3(0, 0, 0));

    buildMeshTBO();

    //setup shader
    _bvhRayTraceShader = shaderManager()->setActiveShader<BVHRayTraceShader>(eShaderProgram_RayTrace);
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
    _modelMesh = make_shared<Model>(RESOURCE_DIR + "/objects/monkey/monkey.obj", vec3(0.75, 0.75, 0.75));

    for (const ModelMesh& mesh : _modelMesh->meshes) {
        buildBVH(mesh.vertices, mesh.indices, BVH_MAX_DEPTH, _bvhNodes, _bvhTriangles);
        break; // TODO: multi mesh
    }

    vector<float> bvhNodeIndices; // [triIndex, triCount, childIndex]
    vector<float> bvhMinBounds;
    vector<float> bvhMaxBounds;
    vector<float> trianglePos;
    vector<float> triangleNormal;

    bvhNodeIndices.reserve(_bvhNodes.size() * 3);
    bvhMinBounds.reserve(_bvhNodes.size() * 3);
    bvhMaxBounds.reserve(_bvhNodes.size() * 3);
    trianglePos.reserve(_bvhTriangles.size() * 9);
    triangleNormal.reserve(_bvhTriangles.size() * 9);

    // Node Info
    for (const auto& node : _bvhNodes) {
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
    for (const auto& tri : _bvhTriangles) {
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

}

void RayTraceScene::renderBVH() {
    static GLuint T_VAO = 0;
    static GLuint T_VBO = 0;
    static GLuint T_EBO = 0;

    if (T_VAO == 0) {
        unsigned int indices[] = {
                // Indices for drawing lines
                0, 1, 1, 2, 2, 3, 3, 0, // Bottom face
                4, 5, 5, 6, 6, 7, 7, 4, // Top face
                0, 4, 1, 5, 2, 6, 3, 7  // Side edges
        };

        vector<float> v;
        vec3 color = vec3(0.0, 0.0, 1.0);
        _bvhNodes[1].aabb.getVertices(v, color);

        glGenVertexArrays(1, &T_VAO);
        glGenBuffers(1, &T_VBO);
        glGenBuffers(1, &T_EBO);

        glBindVertexArray(T_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, T_VBO);
        glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, T_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        //pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        //color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    const mat4& proj = _camera->projMat();
    const mat4& view = _camera->viewMat();
    mat4 world;

    //render model
    auto modelShader = shaderManager()->setActiveShader<BasicShader>(eShaderProgram_Basic);
    modelShader->viewMatUniformMatrix4fv(view.ptr());
    modelShader->projMatUniformMatrix4fv(proj.ptr());
    modelShader->worldMatUniformMatrix4fv(world.ptr(), 1);
    modelShader->worldNormalMatUniformMatrix4fv(world.ptr(), 1);
    _modelMesh->render();


    //render bvh boxes
    glDisable(GL_CULL_FACE);
    auto bvhLineshader = shaderManager()->setActiveShader<SimpleShader>(eShaderProgram_Simple);

    bvhLineshader->setColorUnifrom3f(1.0, 0.0, 0.0);
    bvhLineshader->viewMatUniformMatrix4fv(view.ptr());
    bvhLineshader->projMatUniformMatrix4fv(proj.ptr());

    glBindVertexArray(T_VAO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

//    auto f = [&indices](const BVHNode& node) {
//        vector<float> vertices = node.aabb.toVertices();
//
//    };
//
//    visitBVH(1, _bvhNodes, f, _bvhLeafStartIdx, _bvhLeafLastIdx);
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
    _bvhLeafStartIdx = 1 << (BVH_MAX_DEPTH - 1);
    _bvhLeafLastIdx = 1 << BVH_MAX_DEPTH;

    //test
    renderBVH();
    return;

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    const mat4& proj = _camera->projMat();
    const mat4& view = _camera->viewMat();

    _bvhRayTraceShader->cameraPosUniform3f(_camera->eye().x, _camera->eye().y, _camera->eye().z);
    _bvhRayTraceShader->viewMatUniformMatrix4fv(view.ptr());
    _bvhRayTraceShader->projMatUniformMatrix4fv(proj.ptr());
    _bvhRayTraceShader->bvhLeafStartIdxUniform1i(_bvhLeafStartIdx);

    _fullQuad->render();
}


shared_ptr<ShaderManager> RayTraceScene::shaderManager() {
    return _engine->_shaderManager;
}

RayTraceScene::~RayTraceScene() {
    if (_bvhNodeTBO != 0)
        glDeleteBuffers(1, &_bvhNodeTBO);

    if (_bvhNodeTBOTexture != 0)
        glDeleteTextures(1, &_bvhNodeTBOTexture);

    if (_bvhMinBoundsTBO != 0)
        glDeleteBuffers(1, &_bvhMinBoundsTBO);

    if (_bvhMaxBoundsTBO != 0)
        glDeleteBuffers(1, &_bvhMaxBoundsTBO);

    if (_bvhMinBoundsTBOTexture != 0)
        glDeleteBuffers(1, &_bvhMinBoundsTBOTexture);

    if (_bvhMaxBoundsTBOTexture != 0)
        glDeleteTextures(1, &_bvhMaxBoundsTBOTexture);

    if (_posTBO != 0)
        glDeleteBuffers(1, &_posTBO);

    if (_posTBOTexture != 0)
        glDeleteTextures(1, &_bvhMaxBoundsTBOTexture);

    if (_normalTBO != 0)
        glDeleteBuffers(1, &_normalTBO);

    if (_normalTBOTexture != 0)
        glDeleteTextures(1, &_normalTBOTexture);
}
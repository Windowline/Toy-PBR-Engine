#include "RayTraceScene.hpp"
#include "Engine.hpp"
#include "Camera.hpp"
#include "GLUtilGeometry.hpp"
#include "FullQuad.hpp"

#include "ShaderManager.hpp"
#include "BasicShader.hpp"
#include "RayTraceShader.hpp"
#include "PathInfo.h"

#include "MeshBasic.h"
#include "Model.hpp"
#include "Node.hpp"


using namespace std;

RayTraceScene::RayTraceScene(RenderEngine* engine, GLuint defaultFBO) : _engine(engine), _defaultFBO(defaultFBO) {
    _fullQuad = make_unique<FullQuad>("FullQuad");
    _camera = make_shared<Camera>(vec3(0, 0, 50), vec3(0, 0, 0));

    _rootNode = make_shared<Node>(nullptr, make_shared<MeshBasic>(), mat4());
    _rootNode->setEnabled(false);

    _modelMesh = make_shared<Model>(RESOURCE_DIR + "/objects/monkey/monkey.obj", vec3(0.75, 0.75, 0.75), "Model");
    _modelNode = make_shared<Node>(nullptr, _modelMesh, mat4());
    _rootNode->addChild(_modelNode);



    auto shader = shaderManager()->setActiveShader<RayTraceShader>(eShaderProgram_RayTrace);

    //TBO
//    vector<float> bufferData = { 0.0f, 0.5f, 1.0f, 0.6};
    // 정점 데이터 (예: 2개의 삼각형)
    std::vector<float> bufferData = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f,
            0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
            0.0f, -0.5f, 0.0f
    };

    GLuint TBO;
    glGenBuffers(1, &TBO);
    glBindBuffer(GL_TEXTURE_BUFFER, TBO);
    glBufferData(GL_TEXTURE_BUFFER, bufferData.size() * sizeof(float), bufferData.data(), GL_STATIC_DRAW);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_BUFFER, texture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, TBO);
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
    glClearColor(1.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 camLocal;

    auto shader = shaderManager()->setActiveShader<RayTraceShader>(eShaderProgram_RayTrace);
    shader->cameraPosUniform3f(_camera->eye().x, _camera->eye().y, _camera->eye().z);
    shader->cameraLocalToWorldMatUniformMatrix4fv(camLocal.pointer());
    shader->resolutionUniform2f((float)_camera->screenSize().x, (float)_camera->screenSize().y);

    _fullQuad->render();
}


shared_ptr<ShaderManager> RayTraceScene::shaderManager() {
    return _engine->_shaderManager;
}
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
    // 정점 데이터 (예: 2개의 삼각형)
    vector<float> posBufferData = {
//            -0.5f, -0.5f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f,
            0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
            0.0f, -0.5f, 0.0f
    };

    vector<float> normalBufferData = {
//            -0.5f, -0.5f, 0.0f,
            0.0f, 1.0f, 0.0f,

            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f,
            0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
            0.0f, -0.5f, 0.0f
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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_BUFFER, _posTBOTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, _normalTBOTexture);

    _fullQuad->render();
}


shared_ptr<ShaderManager> RayTraceScene::shaderManager() {
    return _engine->_shaderManager;
}
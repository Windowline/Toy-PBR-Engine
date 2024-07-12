#include "Engine.hpp"
#include "ShaderManager.hpp"
#include "Scene.hpp"

#include "RayTraceScene.hpp"

#include <glad/glad.h>

RenderEngine::RenderEngine() {
    _shaderManager = std::make_shared<ShaderManager>();
}

void RenderEngine::prepare() {
    _shaderManager->loadAllPrograms();
}

void RenderEngine::setScreenSize(int w, int h) {
    _screenSize = ivec2(w, h);
    if (_scene) {
        _scene->setScreenSize(_screenSize.x, _screenSize.y);
    }
}

void RenderEngine::updateViewPosition(int dir, float delta) {
    if (_scene) {
        _scene->updateViewPosition(dir, delta);
    }
}

void RenderEngine::updateViewRotation(float yaw, float pitch) {
    if (_scene) {
        _scene->updateViewRotation(yaw, pitch);
    }
}


void RenderEngine::render() {
    initScene();
    _scene->update();
    _scene->render();
}


void RenderEngine::initScene() {
    if (_init)
        return;

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&_defaultFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBO);
    glViewport(0, 0, _screenSize.x, _screenSize.y);
    glClearColor(1.0f, 1.0f, 1.0f, 1.f);                // Black Background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    _scene = std::make_shared<Scene>(this, _defaultFBO);
    _scene = std::make_shared<RayTraceScene>(this, _defaultFBO);
    _scene->setScreenSize(_screenSize.x, _screenSize.y);
    _init = true;
}





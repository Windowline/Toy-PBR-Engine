#include "Engine.hpp"
#include "ShaderManager.hpp"
#include "Scene.hpp"

#include <glad/glad.h>

RenderEngine::RenderEngine() {
    _shaderManager = std::make_shared<ShaderManager>();
}

void RenderEngine::prepare() {
    _shaderManager->loadAllPrograms();
}

void RenderEngine::setScreenSize(int w, int h) {
    _screenSize = ivec2(w, h);
    if (_scene)
        _scene->setScreenSize(_screenSize.x, _screenSize.y);
}

void RenderEngine::updateViewPosition(int dir, float delta) {
    if (_scene)
        _scene->updateViewPosition(dir, delta);
}

void RenderEngine::updateViewRotation(float yaw, float pitch) {
    if (_scene)
        _scene->updateViewRotation(yaw, pitch);
}


void RenderEngine::render() {
    initScene();

    //test
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // default buffer clear
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _scene->render();
}


void RenderEngine::initScene() {

    if (_init)
        return;

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&_defaultFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBO);
    glViewport(0, 0, _screenSize.x, _screenSize.y);

    glClearColor(1.0f, 1.0f, 1.0f, 1.f);                // Black Background
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    _scene = std::make_shared<Scene>(this, _defaultFBO);
    _scene->setScreenSize(_screenSize.x, _screenSize.y);

    _init = true;
}





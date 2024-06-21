#include "Scene.hpp"
#include "Engine.hpp"
#include "ShaderManager.hpp"
#include "Node.hpp"
#include "Camera.hpp"
#include "FrameBufferObject.hpp"
#include "GLUtilGeometry.hpp"

#include "BasicShader.hpp"
#include "ShadowDepthShader.hpp"
#include "GBufferShader.hpp"
#include "TexturePassShader.hpp"
#include "SSAOShader.hpp"
#include "SSAOBlurShader.hpp"
#include "DeferredLightingShader.hpp"

#include "Cube.hpp"
#include "Sphere.hpp"
#include "Room.hpp"
#include "FullQuad.hpp"
#include "Model.hpp"

#include <random>

using namespace std;

Scene::Scene(RenderEngine* engine, GLuint defaultFBO) : _engine(engine), _defaultFBO(defaultFBO)
{
    _fullQuad = make_unique<FullQuad>();
    _textureShader = make_unique<TexturePassShader>();

    _camera = make_shared<Camera>();

    _lightPositions = {
        vec3(0.f, 44.f, 20.f)
    };

    _ambientColor = vec3(0.4f, 0.4f, 0.4f);
    _specularColor = vec3(0.9f, 0.9f, 0.9f);
    _diffuseColor = vec3(0.5f, 0.5f, 0.5f);


    //방 메시를 생성합니다. 각면의 색상을 달리 받습니다.
    auto roomMesh = make_shared<Room>(100, vec3(0, 1, 0), vec3(0, 0, 0), vec3(1, 0, 0),
                                           vec3(0, 0, 1), vec3(0.5, 0.5, 0.5));

    //파란색 구체 메시를 생성합니다.
    auto sphereMesh = make_shared<Sphere>(16, vec3(0, 0, 1));

    //흰색 정육면체 메시를 생성합니다.
    auto cubeMesh = make_shared<Cube>(20, vec3(1, 1, 1));

    //방 천장에 위치할 빛 구체를 생성합니다. 라이팅, 그림자 등의 연산에선 제외됩니다.
    auto lightSphereMesh = make_shared<Sphere>(4, vec3(1, 1, 1));


    mat4 roomLocalTransform;
    _room = make_shared<Node>(this, roomMesh, roomLocalTransform);


    mat4 sphereLocalTransform = mat4::Translate(-20, -25, 20);
    _sphere = make_shared<Node>(this, sphereMesh, sphereLocalTransform);


    mat4 cubeLocalTransform = mat4::RotateY(45.f) * mat4::Translate(25, -40, -20);
    _cube = make_shared<Node>(this, cubeMesh, cubeLocalTransform);


    mat4 lightSphereLocalTransform = mat4::Translate(_lightPositions.front().x,
                                                     _lightPositions.front().y,
                                                     _lightPositions.front().z);

    _lightSphere = make_shared<Node>(this, lightSphereMesh, lightSphereLocalTransform);
    _lightSphere->transformUpdate();


    mat4 modelLocalTransform = mat4::Scale(6.f) * mat4::Translate(5, -10, 20);
    auto modelMesh = make_shared<Model>("/Users/bagchangseon/CLionProjects/ToyRenderer/lib/res/objects/backpack/backpack.obj", vec3(0.7, 0.7, 0.7));
    _model = make_shared<Node>(this, modelMesh, modelLocalTransform);

    _rootTransformDirty = true;

    _rootNode = _room;
    _rootNode->addChild(_sphere);
    _rootNode->addChild(_cube);
    _rootNode->addChild(_model);
}

Scene::~Scene() {
    if (_noiseTexture != 0)
        glDeleteTextures(1, &_noiseTexture);
}

//스크린 사이즈 변경에 의해 다시 만들어져야 할 것들을 업데이트합니다.
void Scene::setScreenSize(int w, int h) {
    if (!_camera) {
        return;
    }

    glViewport(0, 0, w, h);
    _camera->setScreenRect(Rect{0, 0, w, h});

    _shadowLightView = Camera::createViewMatrix(vec3(0, 0, 0), _lightPositions.front(), vec3(0, 1, 0)); //라이트가 다수일 경우 shadow용 라이트는 첫번째 라이트로 정했습니다.
    _shadowLightProj = mat4::Ortho(-200, 300, -300, 300, -100, 1000);
    _shadowLightViewProjection = _shadowLightView * _shadowLightProj;

    _gBuffer = make_shared<FrameBufferObject>(_camera->screenSize(), _defaultFBO, FrameBufferObject::Type::GBuffer);

    _shadowDepthBuffer = make_shared<FrameBufferObject>(ivec2(2048, 2048), _defaultFBO, FrameBufferObject::Type::Common);

    buildSSAOInfo();
}

void Scene::updateViewPosition(int dir, float delta) {
    if (_camera)
        _camera->updateViewPosition(dir, delta);
}

void Scene::updateViewRotation(float yaw, float pitch) {
    if (_camera)
        _camera->updateViewRotation(yaw, pitch);
}


void Scene::render() {
    if (_rootTransformDirty) {
        visitNodes(_rootNode, [](const shared_ptr<Node>& node) {
            node->transformUpdate();
        });
        _rootTransformDirty = false;
    }

    const mat4& proj = _camera->projMat();
    const mat4& view = _camera->viewMat();

    // depth
    {
        _shadowDepthBuffer->bindWithViewport();
        glClearColor(0.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto wShader = weak_ptr<ShadowDepthShader>(shaderManager()->setActiveShader<ShadowDepthShader>(eShaderProgram_ShadowDepth));

        visitNodes(_rootNode, [this, wShader](const shared_ptr<Node>& node) {
            if (auto shader = wShader.lock()) {
                mat4 shadowMVP = node->worldTransform() * this->shadowLightViewProjection();
                shader->shadowMVPUniformMatrix4fv(shadowMVP.pointer());
                node->render();
            }
        });

    }

    // gbuffer
    {
        _gBuffer->bindWithViewport();
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto wShader = weak_ptr<GBufferShader>(shaderManager()->setActiveShader<GBufferShader>(eShaderProgram_GBuffer));

        visitNodes(_rootNode, [this, proj, view, wShader](const shared_ptr<Node>& node) {
            if (auto shader = wShader.lock()) {
                shader->projMatUniformMatrix4fv(proj.pointer());
                shader->viewMatUniformMatrix4fv(view.pointer());
                shader->worldMatUniformMatrix4fv(node->worldTransform().pointer());
                shader->worldNormalMatUniformMatrix4fv(node->worldTransform().invert().transposed().pointer());
                node->render();
            }
        });

    }

    // SSAO
    {
        _ssaoFBO->bindWithViewport();
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto shader = shaderManager()->setActiveShader<SSAOShader>(eShaderProgram_SSAO);
        shader->viewMatUniformMatrix4fv(view.pointer());
        shader->projMatUniformMatrix4fv(proj.pointer());
        shader->samplesUniformVector(_ssaoKernel);
        shader->screenSizeUniform2f(_camera->screenSize().x, _camera->screenSize().y);

        const int COMPONENT_COUNT = 3;
        array<GLuint, COMPONENT_COUNT> ssaoInputTextures { _gBuffer->gPositionTexture(),
                                                                _gBuffer->gNormalTexture(),
                                                                _noiseTexture };
        for (int i = 0; i < COMPONENT_COUNT; ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, ssaoInputTextures[i]);
        }

        _fullQuad->render();
    }

    //SSAO Blur
    {
        _ssaoBlurFBO->bindWithViewport();
        auto shader = shaderManager()->setActiveShader<SSAOBlurShader>(eShaderProgram_SSAO_BLUR);
        shader->textureSizeUniform2f(_camera->screenSize().x, _camera->screenSize().y);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _ssaoFBO->commonTexture());
        _fullQuad->render();
    }

    //Deferred
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBO);
        glViewport(0, 0, _camera->screenSize().x, _camera->screenSize().y);
        auto shader = shaderManager()->setActiveShader<DeferredLightingShader>(eShaderProgram_DeferredLighting);
        shader->ambientColorUniform3f(ambientColor().x, ambientColor().y, ambientColor().z);
        shader->diffuseColorUniform3f(diffuseColor().x, diffuseColor().y, diffuseColor().z);
        shader->specularColorUniform3f(specularColor().x, specularColor().y, specularColor().z);
        shader->worldLightPosUniform3fVector(lightPositions());
        shader->worldEyePositionUniform3f(camera()->eye().x, camera()->eye().y, camera()->eye().z);
        shader->shadowViewProjectionMatUniformMatrix4fv(_shadowLightViewProjection.pointer());

        const int GBUFFER_COMPONENT_COUNT = 5;
        array<GLuint, GBUFFER_COMPONENT_COUNT> textures {_gBuffer->gPositionTexture(),
                                                              _gBuffer->gNormalTexture(),
                                                              _gBuffer->gAlbedoTexture(),
                                                              _shadowDepthBuffer->commonTexture(),
                                                              _ssaoBlurFBO->commonTexture()
        };

        for (int i = 0; i < GBUFFER_COMPONENT_COUNT; ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures[i]);
        }

        _fullQuad->render();

        //light sphere
        //6 빛 구체 렌더링, 효과를 입히지 않고, 본연의 색만 입힙니다.
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        auto shaderLight = shaderManager()->setActiveShader<BasicShader>(eShaderProgram_Default);
        shaderLight->projMatUniformMatrix4fv(proj.pointer());
        shaderLight->viewMatUniformMatrix4fv(view.pointer());
        shaderLight->worldMatUniformMatrix4fv(_lightSphere->worldTransform().pointer());
        _lightSphere->render();
    }
}



void Scene::renderQuad(unsigned int texture, ivec2 screenSize) { //for debug
    glDisable(GL_CULL_FACE);
    //glDisable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBO);
    glViewport(0, 0, _camera->screenSize().x, _camera->screenSize().y);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _textureShader->useProgram();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    _fullQuad->render();
}

//Screen Space Ambient Occlusion을 위한 정보를 빌드합니다.
void Scene::buildSSAOInfo() {

    auto lerp = [](float a, float b, float f) -> float {
        return a + f * (b - a);
    };

    //occlusion정도를 구하기 위한 접선 공간상의 반구 모양의 분포를 가진 임의의 샘플포인트를 수집합니다.
    std::random_device rd;
    std::mt19937 gen(rd()); // Mersenne Twister 엔진을 사용한 무작위 수 생성기
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);

    std::default_random_engine generator;
    for (unsigned int i = 0; i < 64; ++i) {
        vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) //z:[0, 1] -> 접선공간상 반구 형태로 분포 시키기 위하여 0~1 범위를 갖습니다.
        );

//        sample.normalize();
//        sample *= randomFloats(generator);
//        _ssaoKernel.push_back(sample);
        float scale = (float)i / 64.f;
        sample.normalize();
        sample *= lerp(0.1f, 1.f, scale * scale);
        _ssaoKernel.emplace_back(sample);
    }

    //샘플 포인트에 임의성을 더하기 위해서 접선공간상 임의 회전 벡터를 생성합니다.
    int idx = 0;
    for (int i = 0; i < 16; ++i) {
        _ssaoNoise[idx++] = randomFloats(generator) * 2.0 - 1.0;
        _ssaoNoise[idx++] = randomFloats(generator) * 2.0 - 1.0;
        _ssaoNoise[idx++] = 0.f; //접선공간 상의 z축 회전을 위하여 0
    }

    //임의 벡터는 4x4 텍스처로 repeat하여 반복적으로 사용합니다. (메모리 효율)
    glGenTextures(1, &_noiseTexture);
    glBindTexture(GL_TEXTURE_2D, _noiseTexture);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &_ssaoNoise[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &_ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    _ssaoFBO = make_shared<FrameBufferObject>(_camera->screenSize(), _defaultFBO, FrameBufferObject::Type::Common);
    _ssaoBlurFBO = make_shared<FrameBufferObject>(_camera->screenSize(), _defaultFBO, FrameBufferObject::Type::Common);
}



void Scene::visitNodes(shared_ptr<Node> node, function<void(shared_ptr<Node>)> func) {
    func(node);
    for (auto child : node->children()) {
        visitNodes(child, func);
    }
}


shared_ptr<ShaderManager> Scene::shaderManager() {
    return _engine->_shaderManager;
}

shared_ptr<Camera> Scene::camera() {
    return _camera;
}

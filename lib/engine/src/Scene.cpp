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
#include "BGShader.hpp"
#include "PBRShader.hpp"
#include "DeferredPBRShader.hpp"

#include "Cube.hpp"
#include "Sphere.hpp"
#include "Room.hpp"
#include "FullQuad.hpp"
#include "Model.hpp"
#include "Plane.hpp"

#include "IBLPreprocessor.hpp"
#include "ImageLoader.hpp"
#include "PathInfo.h"

#include <random>

using namespace std;

//////////////////// For Debug ////////////////////////////
unsigned int ___loadSkyboxForDebug___();
//////////////////////////////////////////////////////////

constexpr float Z_ALIGN = 0.f;

Scene::Scene(RenderEngine* engine, GLuint defaultFBO) : _engine(engine), _defaultFBO(defaultFBO)
{
    _fullCube = make_unique<Cube>(2, vec3(1, 1, 1), "FullCube");
    _fullQuad = make_unique<FullQuad>("FullQuad");
    _textureShader = make_unique<TexturePassShader>();

    _camera = make_shared<Camera>(vec3(0, 0, 130), vec3(0, 0, 0));

    _lightPositions = {
            vec3(-2.f, 30.f, -2.f)
    };

    _lightColors = {
        vec3(0.9, 0.8, 0.8)
    };

    auto roomMesh = make_shared<Room>(1, vec3(0, 1, 0), vec3(0, 0, 0), vec3(1, 0, 0),
                                           vec3(0, 0, 1), vec3(0.5, 0.5, 0.5), "Room");
    mat4 roomLocalTransform;
    _room = make_shared<Node>(this, roomMesh, roomLocalTransform);

    // shpere
    auto sphereMesh = make_shared<Sphere>(1, vec3(0, 0, 1), "Sphere");
    mat4 sphereLocalTransform = mat4::Scale(8) * mat4::Translate(0.f, 3.f, 0.f);
    _sphere = make_shared<Node>(this, sphereMesh, sphereLocalTransform);

    // model
    auto modelMesh = make_shared<Model>(RESOURCE_DIR + "/objects/monkey/monkey.obj", vec3(0.7, 0.7, 0.7), "Model");
    mat4 modelLocalTransform = mat4::Scale(6.f) * mat4::Translate(-40, 10.f, Z_ALIGN - 10);
    _model = make_shared<Node>(this, modelMesh, modelLocalTransform);

    // plane
    auto plane = make_shared<Plane>(1, vec3(0.8, 0.8, 0.8), "Plane");
    mat4 planeLocalTransform = mat4::Scale(120.f, 120.f, 1.f) * mat4::RotateX(90.f) * mat4::Translate(0, -20, Z_ALIGN);
    _plane = make_shared<Node>(this, plane, planeLocalTransform);

    //방 천장에 위치할 빛 구체를 생성합니다. 라이팅, 그림자 등의 연산에선 제외됩니다.
    auto lightSphereMesh = make_shared<Sphere>(1, vec3(1, 1, 1), "LightSphere");
    mat4 lightSphereLocalTransform = mat4::Scale(2.f) * mat4::Translate(_lightPositions.front().x,
                                                                           _lightPositions.front().y,
                                                                           _lightPositions.front().z);
    _lightSphere = make_shared<Node>(this, lightSphereMesh, lightSphereLocalTransform);
    _lightSphere->transformUpdate();

    _rootTransformDirty = true;

    /* 만일 쉐도우 뎁스맵 생성시 뎁스버퍼를 붙일 수 없다면, 광원기준 가까운것부터 그려야한다. */
    _rootNode = _room;
    _rootNode->setEnabled(false);
    _rootNode->addChild(_sphere);
    _rootNode->addChild(_model);
    _rootNode->addChild(_plane);

    _iblPreprocessor = make_unique<IBLPreprocessor>(engine->_shaderManager, RESOURCE_DIR + "/textures/hdr/newport_loft.hdr");
    _iblPreprocessor->build();

    _shadowLightView = Camera::createViewMatrix(vec3(0, 0, 0), _lightPositions.front(), vec3(0, 1, 0)); //라이트가 다수일 경우 shadow용 라이트는 첫번째 라이트로 정했습니다.
    _shadowLightProj = mat4::Ortho(-100, 100, -100, 100, 0.1f, 60.f);

    _shadowLightViewProjection = _shadowLightView * _shadowLightProj;
    _shadowDepthBuffer = make_shared<FrameBufferObject>(ivec2(1024, 1024), _defaultFBO, FrameBufferObject::Type::Depth);
}

Scene::~Scene() {
    if (_ssaoNoiseTexture != 0)
        glDeleteTextures(1, &_ssaoNoiseTexture);
}

//스크린 사이즈 변경에 의해 다시 만들어져야 할 것들을 업데이트합니다.
void Scene::setScreenSize(int w, int h) {
    if (!_camera) {
        return;
    }

    glViewport(0, 0, w, h);
    _camera->setScreenRect(Rect{0, 0, w, h});
    _gBuffer = make_shared<FrameBufferObject>(_camera->screenSize(), _defaultFBO, FrameBufferObject::Type::GBuffer);

    buildSSAOInfo();
}

void Scene::updateViewPosition(int dir, float delta) {
    if (_camera) {
        _camera->updateViewPosition(dir, delta);
    }
}

void Scene::updateViewRotation(float yaw, float pitch) {
    if (_camera) {
        _camera->updateViewRotation(yaw, pitch);
    }
}

void Scene::update() {
    if (_rootTransformDirty) {
        visitNodes(_rootNode, [](const shared_ptr<Node>& node) {
            node->transformUpdate();
        });
        _rootTransformDirty = false;
    }


}


void Scene::render() {
//    renderForwardPBR();
    renderDeferredPBR();
}

void Scene::renderDeferredPBR() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBO);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const mat4& proj = _camera->projMat();
    const mat4& view = _camera->viewMat();
    mat4 identity;

    // depth
    {
        _shadowDepthBuffer->bindWithViewport();
        glClearColor(0.f, 0.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto activeShader = shaderManager()->setActiveShader<ShadowDepthShader>(eShaderProgram_ShadowDepth);

        visitNodes(_rootNode, [this, wShader = weak_ptr<ShadowDepthShader>(activeShader)](const shared_ptr<Node>& node) {
            if (auto shader = wShader.lock()) {
                mat4 shadowMVP = node->worldTransform() * _shadowLightView * _shadowLightProj;
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

        auto activeShader = shaderManager()->setActiveShader<GBufferShader>(eShaderProgram_GBuffer);

        //skybox
        glDepthFunc(GL_LEQUAL);
        glDisable(GL_CULL_FACE);

        activeShader->projMatUniformMatrix4fv(proj.pointer());
        activeShader->viewMatUniformMatrix4fv(view.pointer());
        activeShader->worldMatUniformMatrix4fv(identity.pointer());
        activeShader->worldNormalMatUniformMatrix4fv(identity.pointer());
        activeShader->isRenderSkyBokxUniform1f(1.f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _iblPreprocessor->envCubemap());
        _fullCube->render();

        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);

        //geometry
        activeShader->projMatUniformMatrix4fv(proj.pointer());
        activeShader->viewMatUniformMatrix4fv(view.pointer());
        activeShader->isRenderSkyBokxUniform1f(0.f);

        visitNodes(_rootNode, [wShader = weak_ptr<GBufferShader>(activeShader)](const shared_ptr<Node>& node) {
            if (auto shader = wShader.lock()) {
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

        auto activeShader = shaderManager()->setActiveShader<SSAOShader>(eShaderProgram_SSAO);
        activeShader->viewMatUniformMatrix4fv(view.pointer());
        activeShader->projMatUniformMatrix4fv(proj.pointer());
        activeShader->samplesUniformVector(_ssaoKernel);
        activeShader->screenSizeUniform2f(_camera->screenSize().x, _camera->screenSize().y);

        const int COMPONENT_COUNT = 3;
        array<GLuint, COMPONENT_COUNT> ssaoInputTextures {_gBuffer->gPositionTexture(),
                                                          _gBuffer->gNormalTexture(),
                                                          _ssaoNoiseTexture };
        for (int i = 0; i < COMPONENT_COUNT; ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, ssaoInputTextures[i]);
        }

        _fullQuad->render();
    }

    //SSAO Blur
    {
        _ssaoBlurFBO->bindWithViewport();
        auto activeShader = shaderManager()->setActiveShader<SSAOBlurShader>(eShaderProgram_SSAO_BLUR);
        activeShader->textureSizeUniform2f(_camera->screenSize().x, _camera->screenSize().y);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _ssaoFBO->commonTexture());
        _fullQuad->render();
    }

    //Deferred PBR(IBL)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBO);
        glViewport(0, 0, _camera->screenSize().x, _camera->screenSize().y);

        auto activeShader = shaderManager()->setActiveShader<DeferredPBRShader>(eShaderProgram_DeferredPBR);
        //Samplers: GBuffer, Depth, SSAO
        const int GBUFFER_COMPONENT_COUNT = 5;
        array<GLuint, GBUFFER_COMPONENT_COUNT> textures { _gBuffer->gPositionTexture(),
                                                          _gBuffer->gNormalTexture(),
                                                          _gBuffer->gAlbedoTexture(),
                                                          _shadowDepthBuffer->commonTexture(),
                                                          _ssaoBlurFBO->commonTexture() };

        int textureIndex = 0;
        for (; textureIndex < GBUFFER_COMPONENT_COUNT; ++textureIndex) {
            glActiveTexture(GL_TEXTURE0 + textureIndex);
            glBindTexture(GL_TEXTURE_2D, textures[textureIndex]);
        }

        //Samplers: IBL
        glActiveTexture(GL_TEXTURE0 + textureIndex++);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _iblPreprocessor->irradianceCubeMap());
        glActiveTexture(GL_TEXTURE0 + textureIndex++);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _iblPreprocessor->prefilterCubeMap());
        glActiveTexture(GL_TEXTURE0 + textureIndex++);
        glBindTexture(GL_TEXTURE_2D, _iblPreprocessor->brdfLUTTexture());

        activeShader->lightUniform3fVector(_lightPositions, true);
        activeShader->lightUniform3fVector(_lightColors, false);
        activeShader->worldEyePositionUniform3f(camera()->eye().x, camera()->eye().y, camera()->eye().z);
        activeShader->shadowViewProjectionMatUniformMatrix4fv(_shadowLightViewProjection.pointer());
        activeShader->metallicUniform1f(0.9);
        activeShader->roughnessUniform1f(0.1);

        _fullQuad->render();
    }

    //빛 구체, 라이팅을 입히지 않고 본연의 색만 입힙니다.
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    auto activeShader = shaderManager()->setActiveShader<BasicShader>(eShaderProgram_Default);
    activeShader->worldMatUniformMatrix4fv(_lightSphere->worldTransform().pointer());
    activeShader->worldNormalMatUniformMatrix4fv(_lightSphere->worldTransform().invert().transposed().pointer());
    activeShader->viewMatUniformMatrix4fv(view.pointer());
    activeShader->projMatUniformMatrix4fv(proj.pointer());
    _lightSphere->render();
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
    glGenTextures(1, &_ssaoNoiseTexture);
    glBindTexture(GL_TEXTURE_2D, _ssaoNoiseTexture);
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

//////////////////// For Debug ////////////////////////////

void Scene::renderForwardPBR() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // default buffer clear
    glClearColor(0.f, 1.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // renderForwardPBR
    constexpr int LIGHT_COUNT = 4;
    vec3 lightPositions[] = {
            vec3(-10.0f,  10.0f, 10.0f),
            vec3( 10.0f,  10.0f, 10.0f),
            vec3(-10.0f, -10.0f, 10.0f),
            vec3( 10.0f, -10.0f, 10.0f),
    };
    vec3 lightColors[] = {
            vec3(300.0f, 300.0f, 300.0f),
            vec3(300.0f, 300.0f, 300.0f),
            vec3(300.0f, 300.0f, 300.0f),
            vec3(300.0f, 300.0f, 300.0f)
    };

    float* lightPosArray = new float[LIGHT_COUNT * 3];
    float* lightColorArray = new float[LIGHT_COUNT * 3];
    int idxPos = 0;
    int idxColor = 0;

    for (int i = 0; i < 4; ++i) {
        auto& lightPos = lightPositions[i];
        auto& lightColor = lightColors[i];

        lightPosArray[idxPos++] = lightPos.x;
        lightPosArray[idxPos++] = lightPos.y;
        lightPosArray[idxPos++] = lightPos.z;

        lightColorArray[idxColor++] = lightColor.x;
        lightColorArray[idxColor++] = lightColor.y;
        lightColorArray[idxColor++] = lightColor.z;
    }

    const mat4& proj = _camera->projMat();
    const mat4& view = _camera->viewMat();

    auto activeShader = shaderManager()->setActiveShader<PBRShader>(eShaderProgram_PBR);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _iblPreprocessor->irradianceCubeMap());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _iblPreprocessor->prefilterCubeMap());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _iblPreprocessor->brdfLUTTexture());

    activeShader->lightPositionsUniformVec3fv(lightPosArray, LIGHT_COUNT);
    activeShader->lightColorsUniformVec3fv(lightColorArray, LIGHT_COUNT);

    delete[] lightPosArray;
    delete[] lightColorArray;

    activeShader->camPosUniform3f(_camera->eye().x, _camera->eye().y, _camera->eye().z);
    activeShader->metallicUniform1f(0.9);
    activeShader->roughnessUniform1f(0.1);
    activeShader->albedoUniform3f(0.5, 0.0, 0.0);
    activeShader->aoUniform1f(1.f);

    activeShader->projMatUniformMatrix4fv(proj.pointer());
    activeShader->viewMatUniformMatrix4fv(view.pointer());

    //#### render sample spheres
    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5 * _sphere->worldTransform().getScale().x;
    for (int row = 0; row < nrRows; ++row) {
        activeShader->metallicUniform1f((float)row / (float)nrRows);
        for (int col = 0; col < nrColumns; ++col) {
            activeShader->roughnessUniform1f(clamp((float)col / (float)nrColumns, 0.05f, 1.0f));
            mat4 world = mat4::Scale(_sphere->worldTransform().getScale().x) * mat4::Translate((float)(col - (nrColumns / 2)) * spacing, (float)(row - (nrRows / 2)) * spacing, -100.0f);
            activeShader->worldMatUniformMatrix4fv(world.pointer());
            activeShader->worldNormalMatUniformMatrix4fv(world.invert().transposed().pointer());
            _sphere->render();
        }
    }

    //#### render models
//    visitNodes(_rootNode, [wShader = weak_ptr<PBRShader>(activeShader)](const shared_ptr<Node>& node) {
//        if (auto shader = wShader.lock()) {
//            shader->worldMatUniformMatrix4fv(node->worldTransform().pointer());
//            shader->worldNormalMatUniformMatrix4fv(node->worldTransform().invert().transposed().pointer());
//            node->render();
//        }
//    });

    glDisable(GL_CULL_FACE);
    renderSkyBox();
    glDepthFunc(GL_LESS); // set depth function back to default
}


void Scene::renderSkyBox() {
    const mat4& proj = _camera->projMat();
    const mat4& viewRot = _camera->viewRotMat();

    auto activeShader = shaderManager()->setActiveShader<BGShader>(eShaderProgram_BG);
    activeShader->projMatUniformMatrix4fv(proj.pointer());
    activeShader->viewMatUniformMatrix4fv(viewRot.pointer());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _iblPreprocessor->envCubemap());
    _fullCube->render();
}



void Scene::debugIBL() {
//    renderQuad(_iblPreprocessor->hdrTexture(), _camera->screenSize());
//    renderQuad(_iblPreprocessor->irradianceCubeMap(), _camera->screenSize()); // ?
//    renderQuad(_iblPreprocessor->prefilterCubeMap(), _camera->screenSize()); // ?
//    renderQuad(_iblPreprocessor->envCubemap(), _camera->screenSize()); // ?
    renderQuad(_iblPreprocessor->brdfLUTTexture(), _camera->screenSize()); // OK
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

unsigned int ___loadSkyboxForDebug___() {
    vector<string> faces = {
        RESOURCE_DIR + "/textures/skybox/right.jpg",
        RESOURCE_DIR + "/textures/skybox/left.jpg",
        RESOURCE_DIR + "/textures/skybox/top.jpg",
        RESOURCE_DIR + "/textures/skybox/bottom.jpg",
        RESOURCE_DIR + "/textures/skybox/front.jpg",
        RESOURCE_DIR + "/textures/skybox/back.jpg"
    };

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = Stb::loadImageUChar(faces[i].c_str(), &width, &height, &nrChannels, 0);
        assert(data != NULL);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        Stb::free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
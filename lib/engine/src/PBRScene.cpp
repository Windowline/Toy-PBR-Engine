#include "PBRScene.hpp"
#include "ShaderManager.hpp"
#include "ModelNode.hpp"
#include "FrameBufferObject.hpp"
#include "GLUtilGeometry.hpp"
#include "Engine.hpp"

#include "BasicShader.hpp"
#include "ShadowDepthShader.hpp"
#include "GBufferShader.hpp"
#include "TexturePassShader.hpp"
#include "SSAOShader.hpp"
#include "SSAOBlurShader.hpp"
#include "BGShader.hpp"
#include "PBRShader.hpp"
#include "DeferredPBRShader.hpp"

#include "Cube.hpp"
#include "Sphere.hpp"
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

PBRScene::PBRScene(RenderEngine* engine, GLuint defaultFBO) {
    _engine = engine;
    _defaultFBO = defaultFBO;

    _fullCube = make_unique<Cube>(2, vec3(1, 1, 1), "FullCube");
    _fullQuad = make_unique<FullQuad>("FullQuad");
    _textureShader = make_unique<TexturePassShader>();

    _camera = make_shared<Camera>(vec3(0, 0, 130), vec3(0, 0, 0));

    _lightPositions = {
            vec3(-2.f, 32.f, -2.f),
            vec3(10.f, 40.f, 15.f),
            vec3(40.f, 60.f, 25.f),
            vec3(0.1, 0.f, 50.f),
    };

    _lightColors = {
            vec3(300.0f, 300.0f, 300.0f),
            vec3(300.0f, 300.0f, 300.0f),
            vec3(300.0f, 300.0f, 300.0f),
            vec3(300.0f, 300.0f, 300.0f)
    };


    _shadowLightPosition = _lightPositions.front();

    _rootNode = make_shared<ModelNode>(this, make_shared<MeshBasic>(), mat4());
    _rootNode->setEnabled(false);

    constexpr float SPACING_Z = 23.f;
    constexpr float SPACING_X = 23.f;
    constexpr float SPACING_Y = 6.f;
    constexpr float ALIGN_Z = 0.f;
    mat4 scale = mat4::Scale(8.f);

    vector<mat4> instanceSphereTransforms;
    vector<mat4> instanceModelTransforms;
    for (float z = 0.f; z < 3.f; z += 1.f) {
        for (float x = 0.f; x < 3.f; x += 1.f) {
            float ty = (2.f - z);
            instanceSphereTransforms.emplace_back(mat4::Translate(15.f + x * SPACING_X, 6.f + ty * SPACING_Y, ALIGN_Z + z * SPACING_Z));
            instanceModelTransforms.emplace_back(mat4::Translate(-15.f - x * SPACING_X, 6.f + ty * SPACING_Y, ALIGN_Z + z * SPACING_Z));
        }
    }

    auto sphereMesh = make_shared<Sphere>(1, vec3(0.5, 0.2, 1), "Sphere");
    auto modelMesh = make_shared<Model>(RESOURCE_DIR + "/objects/monkey/monkey.obj", vec3(0.75, 0.75, 0.75));
    auto sphere = make_shared<ModelNode>(this, sphereMesh, scale);
    auto model = make_shared<ModelNode>(this, modelMesh, scale);
    sphere->setLocalInstanceTransforms(std::move(instanceSphereTransforms));
    model->setLocalInstanceTransforms(std::move(instanceModelTransforms));
    _rootNode->addChild(sphere);
    _rootNode->addChild(model);

    //instance colors
    const vec3 COLORS[3] = {
            vec3(0.76, 0.8, 0.95),
            vec3(0.9, 0.1, 0.2),
            vec3(0.12, 0.4, 0.9),
    };

    int colorIndex = 0;
    for (int i = 0; i < 9; ++i) {
        colorIndex = (colorIndex + 1) % 3;
        _instanceColors.push_back(COLORS[colorIndex]);
    }

    // plane
    mat4 planeLocalTransform = mat4::Scale(120.f, 120.f, 1.f) * mat4::RotateX(90.f) * mat4::Translate(0, -20, Z_ALIGN);
    auto plane = make_shared<ModelNode>(this, make_shared<Plane>(1, vec3(0.8, 0.8, 0.8), "Plane"), planeLocalTransform);
    _rootNode->addChild(plane);

    //그림자 광원 시각화용
    auto lightSphereMesh = make_shared<Sphere>(1, vec3(1, 1, 1), "LightSphere");
    mat4 lightSphereLocalTransform = mat4::Scale(2.f) * mat4::Translate(_shadowLightPosition.x,
                                                                           _shadowLightPosition.y,
                                                                           _shadowLightPosition.z);

    _lightSphere = make_shared<ModelNode>(this, lightSphereMesh, lightSphereLocalTransform);
    _lightSphere->transformUpdate();
    _lightSphere->setEnabled(false);

    _rootTransformDirty = true;

    _iblPreprocessor = make_unique<IBLPreprocessor>(shaderManager(), RESOURCE_DIR + "/textures/hdr/newport_loft.hdr");
    _iblPreprocessor->build();

    _shadowLightView = Camera::createViewMatrix(vec3(0, 0, 0), _shadowLightPosition, vec3(0, 1, 0)); //라이트가 다수일 경우 shadow용 라이트는 첫번째 라이트로 정했습니다.
    _shadowLightProj = mat4::Ortho(-100, 100, -100, 100, 0.1f, 60.f);
    _shadowLightViewProjection = _shadowLightView * _shadowLightProj;
    _shadowDepthBuffer = make_shared<FrameBufferObject>(ivec2(1024, 1024), _defaultFBO, FrameBufferObject::Type::Depth);

    buildSSAOInfo();
}

PBRScene::~PBRScene() {
    if (_ssaoNoiseTexture != 0)
        glDeleteTextures(1, &_ssaoNoiseTexture);
}

//스크린 사이즈 변경에 의해 다시 만들어져야 할 것들을 업데이트합니다.
void PBRScene::setScreenSize(int w, int h) {
    if (!_camera) {
        return;
    }

    glViewport(0, 0, w, h);
    _camera->setScreenRect(Rect{0, 0, w, h});
    _gBuffer = make_shared<FrameBufferObject>(_camera->screenSize(), _defaultFBO, FrameBufferObject::Type::GBuffer);
    _ssaoFBO = make_shared<FrameBufferObject>(_camera->screenSize(), _defaultFBO, FrameBufferObject::Type::Common);
    _ssaoBlurFBO = make_shared<FrameBufferObject>(_camera->screenSize(), _defaultFBO, FrameBufferObject::Type::Common);
}

void PBRScene::updateViewPosition(int dir, float delta) {
    if (_camera) {
        _camera->updateViewPosition(dir, delta);
    }
}

void PBRScene::updateViewRotation(float yaw, float pitch) {
    if (_camera) {
        _camera->updateViewRotation(yaw, pitch);
    }
}

void PBRScene::update() {
    if (_rootTransformDirty) {
        visitNodes(_rootNode, [](const shared_ptr<ModelNode>& node) {
            node->transformUpdate();
        });
        _rootTransformDirty = false;
    }
}


void PBRScene::render() {
    renderDeferredPBR();
}

void PBRScene::renderDeferredPBR() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBO);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const mat4& proj = _camera->projMat();
    const mat4& invProj = proj.invert();
    const mat4& view = _camera->viewMat();
    const mat4& invView = view.invert();
    mat4 identity;

    // shadow depth buffer
    {
        _shadowDepthBuffer->bindWithViewport();
        glClearColor(0.f, 0.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto activeShader = shaderManager()->setActiveShader<ShadowDepthShader>(eShaderProgram_ShadowDepth);

        mat4 shadowVP = _shadowLightView * _shadowLightProj;

        visitNodes(_rootNode, [&shadowVP, wShader = weak_ptr<ShadowDepthShader>(activeShader)](const shared_ptr<ModelNode>& node) {
            if (auto shader = wShader.lock()) {
                if (node->isInstancing()) {
                    vector<mat4> shadowMVPInstances = std::move(node->worldInstanceTransformsCombind(shadowVP));
                    shader->shadowMVPUniformMatrix4fv(shadowMVPInstances.data()->ptr(), shadowMVPInstances.size());
                } else {
                    mat4 shadowMVP = node->worldTransform() * shadowVP;
                    shader->shadowMVPUniformMatrix4fv(shadowMVP.ptr(), 1);
                }
                node->render();
            }
        });
    }

    // G buffer
    {
        _gBuffer->bindWithViewport();
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto activeShader = shaderManager()->setActiveShader<GBufferShader>(eShaderProgram_GBuffer);

        //skybox
        glDepthFunc(GL_LEQUAL);
        glDisable(GL_CULL_FACE);

        activeShader->projMatUniformMatrix4fv(proj.ptr());
        activeShader->viewMatUniformMatrix4fv(view.ptr());
        activeShader->worldMatUniformMatrix4fv(identity.ptr(), 1);
        activeShader->worldNormalMatUniformMatrix4fv(identity.ptr(), 1);
        activeShader->isRenderSkyBokxUniform1f(1.f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _iblPreprocessor->envCubemap());

        _fullCube->render();

        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);

        //geometry
        activeShader->projMatUniformMatrix4fv(proj.ptr());
        activeShader->viewMatUniformMatrix4fv(view.ptr());
        activeShader->isRenderSkyBokxUniform1f(0.f);

        visitNodes(_rootNode, [this, wShader = weak_ptr<GBufferShader>(activeShader)](const shared_ptr<ModelNode>& node) {
            if (auto shader = wShader.lock()) {
                if (node->isInstancing()) {
                    shader->colorUniform3fv(&_instanceColors[0].x, node->instanceCount());
                    shader->worldMatUniformMatrix4fv(node->worldInstanceTransforms().data()->ptr(), node->instanceCount());
                    shader->worldNormalMatUniformMatrix4fv(node->worldInstanceNormalTransforms().data()->ptr(), node->instanceCount());
                } else {
                    shader->worldMatUniformMatrix4fv(node->worldTransform().ptr(), 1);
                    shader->worldNormalMatUniformMatrix4fv(node->worldTransform().invert().transposed().ptr(), 1);
                }
                node->render();
            }
        });
    }

    // Screen Space Ambient Occlusion
    {
        _ssaoFBO->bindWithViewport();
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto activeShader = shaderManager()->setActiveShader<SSAOShader>(eShaderProgram_SSAO);
        activeShader->projMatUniformMatrix4fv(proj.ptr());
        activeShader->invProjMatUniformMatrix4fv(invProj.ptr());
        activeShader->samplesUniformVector(_ssaoKernel);
        activeShader->screenSizeUniform2f(_camera->screenSize().x, _camera->screenSize().y);

        const int COMPONENT_COUNT = 2;
        array<GLuint, COMPONENT_COUNT> ssaoInputTextures {
            _gBuffer->gViewNormalTexture(),
            _ssaoNoiseTexture
        };

        for (int i = 0; i < COMPONENT_COUNT; ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, ssaoInputTextures[i]);
        }

        _fullQuad->render();
    }

    // Blur On Screen Space Ambient Occlusion
    {
        _ssaoBlurFBO->bindWithViewport();
        auto activeShader = shaderManager()->setActiveShader<SSAOBlurShader>(eShaderProgram_SSAO_BLUR);
        activeShader->textureSizeUniform2f(_camera->screenSize().x, _camera->screenSize().y);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _ssaoFBO->commonTexture());
        _fullQuad->render();
    }

    //Deferred PBR
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBO);
        glViewport(0, 0, _camera->screenSize().x, _camera->screenSize().y);

        auto activeShader = shaderManager()->setActiveShader<DeferredPBRShader>(eShaderProgram_DeferredPBR);
        //Samplers: GBuffer, Depth, SSAO
        const int GBUFFER_COMPONENT_COUNT = 4;
        array<GLuint, GBUFFER_COMPONENT_COUNT> textures {
            _gBuffer->gNormalTexture(),
            _gBuffer->gAlbedoTexture(),
            _shadowDepthBuffer->commonTexture(),
            _ssaoBlurFBO->commonTexture()
        };

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
        activeShader->shadowViewProjectionMatUniformMatrix4fv(_shadowLightViewProjection.ptr());
        activeShader->metallicUniform1f(0.92);
        activeShader->roughnessUniform1f(0.08);
        activeShader->invProjMatUniformMatrix4fv(invProj.ptr());
        activeShader->invViewMatUniformMatrix4fv(invView.ptr());
        _fullQuad->render();
    }

    //빛 구체, 라이팅을 입히지 않고 본연의 색만 입힙니다.
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    auto activeShader = shaderManager()->setActiveShader<BasicShader>(eShaderProgram_Basic);
    activeShader->worldMatUniformMatrix4fv(_lightSphere->worldTransform().ptr(), 1);
    activeShader->worldNormalMatUniformMatrix4fv(_lightSphere->worldTransform().invert().transposed().ptr(), 1);
    activeShader->viewMatUniformMatrix4fv(view.ptr());
    activeShader->projMatUniformMatrix4fv(proj.ptr());
    _lightSphere->render();
}

//Screen Space Ambient Occlusion을 위한 정보를 빌드합니다.
void PBRScene::buildSSAOInfo() {
    auto lerp = [](float a, float b, float f) -> float {
        return a + f * (b - a);
    };

    //occlusion정도를 구하기 위한 접선 공간상의 반구 모양의 분포를 가진 임의의 샘플포인트를 수집합니다.
    std::random_device rd;
    std::mt19937 gen(rd()); // Mersenne Twister 엔진을 사용한 무작위 수 생성기
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    for (int i = 0; i < 64; ++i) {
        vec3 sample = vec3(randomFloats(generator) * 2.0 - 1.0,
                           randomFloats(generator) * 2.0 - 1.0,
                           randomFloats(generator) //z:[0, 1] -> 접선공간상 반구 형태로 분포 시키기 위하여 0~1 범위를 갖습니다.
                            ).normalized();

        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
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
}

void PBRScene::visitNodes(shared_ptr<ModelNode> node, function<void(shared_ptr<ModelNode>)> func) {
    func(node);
    for (auto child : node->children()) {
        visitNodes(child, func);
    }
}


shared_ptr<ShaderManager> PBRScene::shaderManager() {
    return _engine->_shaderManager;
}

shared_ptr<Camera> PBRScene::camera() {
    return _camera;
}

//////////////////// For Debug ////////////////////////////

void PBRScene::renderForwardPBR() {
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

    const mat4& proj = _camera->projMat();
    const mat4& view = _camera->viewMat();

    auto activeShader = shaderManager()->setActiveShader<PBRShader>(eShaderProgram_PBR);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _iblPreprocessor->irradianceCubeMap());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _iblPreprocessor->prefilterCubeMap());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _iblPreprocessor->brdfLUTTexture());

    activeShader->lightPositionsUniformVec3fv(&lightPositions[0].x, LIGHT_COUNT);
    activeShader->lightColorsUniformVec3fv(&lightColors[0].x, LIGHT_COUNT);
    activeShader->camPosUniform3f(_camera->eye().x, _camera->eye().y, _camera->eye().z);
    activeShader->metallicUniform1f(0.9);
    activeShader->roughnessUniform1f(0.1);
    activeShader->albedoUniform3f(0.5, 0.0, 0.0);
    activeShader->aoUniform1f(1.f);

    activeShader->projMatUniformMatrix4fv(proj.ptr());
    activeShader->viewMatUniformMatrix4fv(view.ptr());

    visitNodes(_rootNode, [wShader = weak_ptr<PBRShader>(activeShader)](const shared_ptr<ModelNode>& node) {
        if (auto shader = wShader.lock()) {
            shader->worldMatUniformMatrix4fv(node->worldTransform().ptr(), 1);
            shader->worldNormalMatUniformMatrix4fv(node->worldTransform().invert().transposed().ptr(), 1);
            node->render();
        }
    });

    glDisable(GL_CULL_FACE);
    renderSkyBox();
    glDepthFunc(GL_LESS); // set depth function back to default
}


void PBRScene::renderSkyBox() {
    const mat4& proj = _camera->projMat();
    const mat4& viewRot = _camera->viewRotMat();

    auto activeShader = shaderManager()->setActiveShader<BGShader>(eShaderProgram_BG);
    activeShader->projMatUniformMatrix4fv(proj.ptr());
    activeShader->viewMatUniformMatrix4fv(viewRot.ptr());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _iblPreprocessor->envCubemap());
    _fullCube->render();
}



void PBRScene::debugIBL() {
//    renderQuad(_iblPreprocessor->hdrTexture(), _camera->screenSize());
//    renderQuad(_iblPreprocessor->irradianceCubeMap(), _camera->screenSize()); // ?
//    renderQuad(_iblPreprocessor->prefilterCubeMap(), _camera->screenSize()); // ?
//    renderQuad(_iblPreprocessor->envCubemap(), _camera->screenSize()); // ?
    renderQuad(_iblPreprocessor->brdfLUTTexture(), _camera->screenSize()); // OK
}

void PBRScene::renderQuad(unsigned int texture, ivec2 screenSize) { //for debug
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
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

#include "Cube.hpp"
#include "Sphere.hpp"
#include "Room.hpp"
#include "FullQuad.hpp"
#include "Model.hpp"

#include "IBLPreprocessor.hpp"
#include "ImageLoader.hpp"

#include <random>

using namespace std;

void renderCube___();
unsigned int loadSkybox___();
unsigned int loadCubemap___(vector<std::string> faces);

Scene::Scene(RenderEngine* engine, GLuint defaultFBO) : _engine(engine), _defaultFBO(defaultFBO)
{
    _fullQuad = make_unique<FullQuad>();
    _textureShader = make_unique<TexturePassShader>();

//    _camera = make_shared<Camera>(vec3(0, 0, 150), vec3(0, 0, 0));
    _camera = make_shared<Camera>(vec3(0, 0, 0), vec3(0, 0, -1));
//    _camera = make_shared<Camera>(vec3(0, 0, 0), vec3(0, 0, 1));


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
//    auto modelMesh = make_shared<Model>("/Users/bagchangseon/CLionProjects/ToyRenderer/lib/res/objects/backpack/backpack.obj", vec3(0.7, 0.7, 0.7));
//    _model = make_shared<Node>(this, modelMesh, modelLocalTransform);

    _rootTransformDirty = true;

    _rootNode = _room;
    _rootNode->addChild(_sphere);
    _rootNode->addChild(_cube);
//    _rootNode->addChild(_model);

    _iblPreprocessor = make_unique<IBLPreprocessor>(engine->_shaderManager, "/Users/bagchangseon/CLionProjects/ToyRenderer/lib/res/textures/hdr/newport_loft.hdr");
//    _iblPreprocessor->build();

//    _rootNode->setEnabled(false);
//    _model->setEnabled(false);

    buildSkyBoxVAO();
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
    //test
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
//    glFrontFace(GL_CCW);
//    glCullFace(GL_BACK);


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // default buffer clear
    glClearColor(0.f, 1.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderSkyBox();

    glDepthFunc(GL_LESS); // set depth function back to default

    return;


    if (_rootTransformDirty) {
        visitNodes(_rootNode, [](const shared_ptr<Node>& node) {
            node->transformUpdate();
        });
        _rootTransformDirty = false;
    }

    const mat4& proj = _camera->projMat();
    const mat4& view = _camera->viewMat();
    const mat4& shadowLightViewProj = shadowLightViewProjection();

    // depth
    {
        _shadowDepthBuffer->bindWithViewport();
        glClearColor(0.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto activeShader = shaderManager()->setActiveShader<ShadowDepthShader>(eShaderProgram_ShadowDepth);

        visitNodes(_rootNode, [shadowLightViewProj, wShader = weak_ptr<ShadowDepthShader>(activeShader)](const shared_ptr<Node>& node) {
            if (auto shader = wShader.lock()) {
                mat4 shadowMVP = node->worldTransform() * shadowLightViewProj;
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
        activeShader->projMatUniformMatrix4fv(proj.pointer());
        activeShader->viewMatUniformMatrix4fv(view.pointer());

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

    //Deferred
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBO);
        glViewport(0, 0, _camera->screenSize().x, _camera->screenSize().y);
        auto activeShader = shaderManager()->setActiveShader<DeferredLightingShader>(eShaderProgram_DeferredLighting);
        activeShader->ambientColorUniform3f(ambientColor().x, ambientColor().y, ambientColor().z);
        activeShader->diffuseColorUniform3f(diffuseColor().x, diffuseColor().y, diffuseColor().z);
        activeShader->specularColorUniform3f(specularColor().x, specularColor().y, specularColor().z);
        activeShader->worldLightPosUniform3fVector(lightPositions());
        activeShader->worldEyePositionUniform3f(camera()->eye().x, camera()->eye().y, camera()->eye().z);
        activeShader->shadowViewProjectionMatUniformMatrix4fv(_shadowLightViewProjection.pointer());

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

void Scene::renderSkyBox() {

    static unsigned int cubemapTexture = 5123;
    if (cubemapTexture == 5123) {
        cubemapTexture = loadSkybox___();
    }
    
    const mat4 proj = _camera->projMat();
    const mat4& viewRot = _camera->viewRotMat();

    auto activeShader = shaderManager()->setActiveShader<BGShader>(eShaderProgram_BG);
    activeShader->projMatUniformMatrix4fv(proj.pointer());
    activeShader->viewMatUniformMatrix4fv(viewRot.pointer());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

//    glBindTexture(GL_TEXTURE_CUBE_MAP, _iblPreprocessor->envCubemap());
//    glBindTexture(GL_TEXTURE_CUBE_MAP, _iblPreprocessor->irradianceMap()); // display irradiance map
//    glBindTexture(GL_TEXTURE_CUBE_MAP, _iblPreprocessor->prefilterMap()); // display prefilter map
    renderCube___();
}

void Scene::renderPBR() {
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


    if (_rootTransformDirty) {
        visitNodes(_rootNode, [](const shared_ptr<Node>& node) {
            node->transformUpdate();
        });
        _rootTransformDirty = false;
    }

    const mat4& proj = _camera->projMat();
    const mat4& view = _camera->viewMat();

    auto activeShader = shaderManager()->setActiveShader<PBRShader>(eShaderProgram_PBR);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, _iblPreprocessor->irradianceMap());
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, _iblPreprocessor->prefilterMap());
//    glActiveTexture(GL_TEXTURE2);
//    glBindTexture(GL_TEXTURE_2D, _iblPreprocessor->brdfLUTTexture());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _iblPreprocessor->irradianceMap());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _iblPreprocessor->prefilterMap());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _iblPreprocessor->brdfLUTTexture());


    activeShader->lightPositionsUniformVec3fv(lightPosArray, LIGHT_COUNT);
    activeShader->lightColorsUniformVec3fv(lightColorArray, LIGHT_COUNT);

    delete[] lightPosArray;
    delete[] lightColorArray;

    activeShader->projMatUniformMatrix4fv(proj.pointer());
    activeShader->viewMatUniformMatrix4fv(view.pointer());
    activeShader->camPosUniform3f(_camera->eye().x, _camera->eye().y, _camera->eye().z);
    activeShader->metallicUniform1f(0.5);
    activeShader->roughnessUniform1f(0.5);
    activeShader->albedoUniform3f(0.5, 0.0, 0.0);
    activeShader->aoUniform1f(1.f);

    visitNodes(_rootNode, [this, proj, view, wShader = weak_ptr<PBRShader>(activeShader)](const shared_ptr<Node>& node) {
        if (auto shader = wShader.lock()) {
            shader->worldMatUniformMatrix4fv(node->worldTransform().pointer());
            shader->worldNormalMatUniformMatrix4fv(node->worldTransform().invert().transposed().pointer());
            node->render();
        }
    });
}

void Scene::debugIBL() {
//    renderQuad(_iblPreprocessor->hdrTexture(), _camera->screenSize());
//    renderQuad(_iblPreprocessor->irradianceMap(), _camera->screenSize()); // ?
//    renderQuad(_iblPreprocessor->prefilterMap(), _camera->screenSize()); // ?
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



unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube___()
{



    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
                // back face
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
                // bottom face
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                // top face
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
                1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}


void Scene::buildSkyBoxVAO() {
    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };


    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

unsigned int loadSkybox___() {
    vector<string> faces =
    {
        std::string("/Users/bagchangseon/CLionProjects/ToyRenderer/lib/res/textures/skybox/right.jpg"),
        std::string("/Users/bagchangseon/CLionProjects/ToyRenderer/lib/res/textures/skybox/left.jpg"),
        std::string("/Users/bagchangseon/CLionProjects/ToyRenderer/lib/res/textures/skybox/top.jpg"),
        std::string("/Users/bagchangseon/CLionProjects/ToyRenderer/lib/res/textures/skybox/bottom.jpg"),
        std::string("/Users/bagchangseon/CLionProjects/ToyRenderer/lib/res/textures/skybox/front.jpg"),
        std::string("/Users/bagchangseon/CLionProjects/ToyRenderer/lib/res/textures/skybox/back.jpg")
    };

   return loadCubemap___(faces);
}

unsigned int loadCubemap___(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = Stb::loadImageUChar(faces[i].c_str(), &width, &height, &nrChannels, 0);
        assert(data != NULL);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//        Stb::free(data)
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

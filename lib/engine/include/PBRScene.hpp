
#ifndef Scene_hpp
#define Scene_hpp
#include <functional>
#include "Scene.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"

class Camera;
class RenderEngine;
class ModelNode;
class ShaderManager;
class FrameBufferObject;
class FullQuad;
class TexturePassShader;
class IBLPreprocessor;
class Cube;

class PBRScene : public Scene {

public:
    PBRScene(RenderEngine* engine, unsigned int defaultFBO);

    virtual ~PBRScene();

    void setScreenSize(int w, int h) override;

    void updateViewPosition(int dir, float delta) override;

    void updateViewRotation(float yaw, float pitch) override;

    void update() override;

    void render() override;

    std::shared_ptr<ShaderManager> shaderManager();
    std::shared_ptr<Camera> camera();

private:
    void renderDeferredPBR();
    void visitNodes(std::shared_ptr<ModelNode> node, std::function<void(std::shared_ptr<ModelNode>)> func);
    void buildSSAOInfo();
    void setupIntanceInfo();

    //for debug
    void renderForwardPBR();
    void renderSkyBox();
    void debugIBL();
    void renderQuad(unsigned int texture, ivec2 screenSize);

    RenderEngine* _engine;
    unsigned int _defaultFBO;
    std::shared_ptr<Camera> _camera;

    bool _rootTransformDirty;

    std::shared_ptr<ModelNode> _rootNode;
    std::shared_ptr<ModelNode> _plane;
    std::shared_ptr<ModelNode> _lightSphere;//조명에 위치한 흰색 구
    std::shared_ptr<FrameBufferObject> _gBuffer;
    std::shared_ptr<FrameBufferObject> _shadowDepthBuffer;

    mat4 _shadowLightView;
    mat4 _shadowLightProj;
    mat4 _shadowLightViewProjection;

    vec3 _shadowLightPosition;
    std::vector<vec3> _lightPositions;
    std::vector<vec3> _lightColors;

    //ssao
    std::vector<vec3> _ssaoKernel;
    float _ssaoNoise[16*3];
    unsigned int _ssaoNoiseTexture;
    std::shared_ptr<FrameBufferObject> _ssaoFBO;
    std::shared_ptr<FrameBufferObject> _ssaoBlurFBO;

    std::unique_ptr<FullQuad> _fullQuad;
    std::unique_ptr<Cube> _fullCube;
    std::unique_ptr<TexturePassShader> _textureShader;
    std::unique_ptr<IBLPreprocessor> _iblPreprocessor;

    //instances
    std::vector<mat4> _instanceSphereTransforms;
    std::vector<mat4> _instanceModelTransforms;
    std::vector<vec3> _instanceColors;
};

#endif /* Scene_hpp */
